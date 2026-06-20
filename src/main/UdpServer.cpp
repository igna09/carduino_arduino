#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#include "Definitions.h"

// --- CONFIGURAZIONE ---
#define WIFI_CHANNEL            6
#define MAX_STA_CONN            5
#define UDP_BUFFER_SIZE         512

static const char *TAG = "AutoAP";

// --- GESTORE EVENTI WIFI ---
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI(TAG, "SoftAP avviato. SSID: %s | Canale: %d", UDP_LOG_WIFI_SSID, WIFI_CHANNEL);
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Stazione connessa - MAC: " MACSTR " | AID: %d", 
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Stazione disconnessa - MAC: " MACSTR " | AID: %d", 
                 MAC2STR(event->mac), event->aid);
    }
}

// --- INIZIALIZZAZIONE WIFI (SoftAP) ---
static void wifi_init_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    // Crea l'interfaccia di rete di default per SoftAP (DHCP server integrato)
    esp_netif_t* netif = esp_netif_create_default_wifi_ap();
    
    // Configurazione IP statica per il gateway dell'AP (Default: 192.168.4.1)
    esp_netif_ip_info_t ip_info;
    IP4_ADDR(&ip_info.ip, 192, 168, 4, 1);
    IP4_ADDR(&ip_info.gw, 192, 168, 4, 1);
    IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);
    esp_netif_dhcps_stop(netif);
    esp_netif_set_ip_info(netif, &ip_info);
    esp_netif_dhcps_start(netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {};
    auto& ap = wifi_config.ap;
    strlcpy((char*)ap.ssid, UDP_LOG_WIFI_SSID, sizeof(ap.ssid));
    strlcpy((char*)ap.password, UDP_LOG_WIFI_PASSWORD, sizeof(ap.password));
    ap.ssid_len = strlen(UDP_LOG_WIFI_SSID);
    ap.channel = WIFI_CHANNEL;
    ap.max_connection = MAX_STA_CONN;
    ap.authmode = WIFI_AUTH_WPA2_PSK;

    if (strlen(UDP_LOG_WIFI_PASSWORD) == 0) {
        ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// --- TASK RICEZIONE UDP ---
static void udp_server_task(void *pvParameters) {
    char rx_buffer[UDP_BUFFER_SIZE];
    struct sockaddr_storage source_addr;
    socklen_t socklen = sizeof(source_addr);

    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(UDP_LOG_PORT);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(TAG, "Impossibile creare il socket: errno %d. Riprovo...", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Errore bind del socket: errno %d. Riprovo...", errno);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "Ascolto UDP attivo sulla porta %d", UDP_LOG_PORT);

        while (1) {
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            if (len < 0) {
                ESP_LOGE(TAG, "Errore di ricezione (recvfrom): errno %d", errno);
                break; // Rompe il ciclo interno per ricreare il socket in modo pulito
            } else {
                rx_buffer[len] = 0; // Null-terminate per sicurezza stringa
                
                // Estrazione IP del mittente per il log
                char ip_str[32];
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, ip_str, sizeof(ip_str) - 1);
                }
                
                // Stampa minimale dei messaggi sul bus locale/debug
                ESP_LOGI(TAG, "[UDP %s:%d]: %s", ip_str, UDP_LOG_PORT, rx_buffer);
            }
        }

        if (sock != -1) {
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

// --- APP_MAIN ---
extern "C" void app_main(void) {
    // Inizializzazione NVS per i dati di calibrazione PHY del WiFi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Forza il livello globale di log a INFO per ridurre l'overhead sulla seriale
    esp_log_level_set("*", ESP_LOG_INFO);

    // Avvio SoftAP
    wifi_init_softap();

    // Creazione del task UDP dedicato
    xTaskCreatePinnedToCore(udp_server_task, "udp_log_server", 4096, NULL, 5, NULL, tskNO_AFFINITY);
}