#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#include "NodeLog.h"
#include "Definitions.h"
#include "WebLogServer.h"

// --- CONFIGURAZIONE ---
#define WIFI_CHANNEL            6
#define MAX_STA_CONN            5
#define UDP_BUFFER_SIZE         512

// --- CONFIGURAZIONE FILTRO NODI ---
#define MAX_FILTER_NODES        8     // Numero massimo di nodi filtrabili contemporaneamente
#define MAX_NODE_NAME_LEN       32    // Lunghezza massima di un nome nodo
#define CONSOLE_CMD_BUF_SIZE    128   // Buffer per la riga di comando letta da console

// ---------------------------------------------------------------------------
// FILTRO NODI
//
// Stato condiviso tra il task console (scrittura) e il task UDP (lettura).
// Protetto da un mutex perche' i due task girano su core/contesti diversi.
// Nessuna allocazione dinamica: lista a dimensione fissa, coerente con lo
// stile del resto del firmware.
// ---------------------------------------------------------------------------
static char s_filterNodes[MAX_FILTER_NODES][MAX_NODE_NAME_LEN];
static int s_filterCount = 0;          // 0 = filtro disattivato, mostra tutto
static SemaphoreHandle_t s_filterMutex = nullptr;

// Verifica se un nome nodo passa il filtro attualmente impostato.
// Se il filtro e' vuoto (s_filterCount == 0) tutti i nodi passano.
static bool nodePassesFilter(const char* nodeName) {
    if (nodeName == nullptr) {
        // Nessun nome nodo estratto dal pacchetto (fallback IP): se il
        // filtro e' attivo non possiamo sapere a chi appartenga, quindi
        // lo scartiamo per coerenza con "mostra solo i nodi filtrati".
        bool filterActive;
        xSemaphoreTake(s_filterMutex, portMAX_DELAY);
        filterActive = (s_filterCount > 0);
        xSemaphoreGive(s_filterMutex);
        return !filterActive;
    }

    bool pass = false;
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);
    if (s_filterCount == 0) {
        pass = true;
    } else {
        for (int i = 0; i < s_filterCount; i++) {
            if (strcmp(s_filterNodes[i], nodeName) == 0) {
                pass = true;
                break;
            }
        }
    }
    xSemaphoreGive(s_filterMutex);
    return pass;
}

// Imposta il filtro a partire da una lista di nomi separati da virgola
// (es. "Node1,Node2"). Spazi attorno ai nomi vengono ignorati.
static void setFilterFromList(char* list) {
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);
    s_filterCount = 0;

    char* token = strtok(list, ",");
    while (token != nullptr && s_filterCount < MAX_FILTER_NODES) {
        // Trim spazi iniziali
        while (*token == ' ' || *token == '\t') {
            token++;
        }
        // Trim spazi finali
        size_t tlen = strlen(token);
        while (tlen > 0 && (token[tlen - 1] == ' ' || token[tlen - 1] == '\t')) {
            token[--tlen] = 0;
        }

        if (tlen > 0) {
            strlcpy(s_filterNodes[s_filterCount], token, MAX_NODE_NAME_LEN);
            s_filterCount++;
        }

        token = strtok(nullptr, ",");
    }

    int appliedCount = s_filterCount;
    xSemaphoreGive(s_filterMutex);

    if (appliedCount == 0) {
        NLOGI("Filtro vuoto/non valido: nessun filtro applicato, mostro tutti i nodi");
    } else {
        NLOGI("Filtro impostato su %d nodo/i", appliedCount);
    }
}

// Rimuove il filtro: tutti i log torneranno visibili.
static void clearFilter() {
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);
    s_filterCount = 0;
    xSemaphoreGive(s_filterMutex);
    NLOGI("Filtro rimosso: mostro i log di tutti i nodi");
}

// Stampa l'elenco dei nodi attualmente nel filtro (o "nessuno").
static void printFilter() {
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);
    if (s_filterCount == 0) {
        NLOGI("Nessun filtro attivo: mostro i log di tutti i nodi");
    } else {
        NLOGI("Filtro attivo (%d nodo/i):", s_filterCount);
        for (int i = 0; i < s_filterCount; i++) {
            NLOGI("  - %s", s_filterNodes[i]);
        }
    }
    xSemaphoreGive(s_filterMutex);
}

// Stampa i comandi disponibili sulla console.
static void printConsoleHelp() {
    NLOGI("Comandi console disponibili:");
    NLOGI("  filter <nodo1,nodo2,...>  - mostra solo i log dei nodi indicati");
    NLOGI("  clear                     - rimuove il filtro (mostra tutti i nodi)");
    NLOGI("  list                      - mostra il filtro attualmente attivo");
    NLOGI("  help                      - mostra questo elenco");
}

// ---------------------------------------------------------------------------
// TASK CONSOLE: legge righe di comando da stdin (UART di default su ESP-IDF)
// e aggiorna il filtro nodi di conseguenza. Bloccante su getchar(), quindi
// gira su un task dedicato per non interferire col task UDP.
// ---------------------------------------------------------------------------
static void console_task(void *pvParameters) {
    char cmdBuf[CONSOLE_CMD_BUF_SIZE];
    int idx = 0;

    printConsoleHelp();

    while (1) {
        int c = getchar();

        if (c == EOF) {
            // Nessun carattere disponibile in questo momento: la console
            // UART di ESP-IDF e' non bloccante per default, quindi senza
            // questa pausa il task girerebbe a vuoto saturando la CPU.
            vTaskDelay(pdMS_TO_TICKS(20));
            continue;
        }

        if (c == '\n' || c == '\r') {
            if (idx == 0) {
                continue; // riga vuota, ignora
            }
            cmdBuf[idx] = 0;
            idx = 0;

            // Tokenizza: primo token = comando, resto = argomenti
            char* cmd = cmdBuf;
            while (*cmd == ' ' || *cmd == '\t') {
                cmd++;
            }
            char* args = strchr(cmd, ' ');
            if (args != nullptr) {
                *args = 0;
                args++;
                while (*args == ' ' || *args == '\t') {
                    args++;
                }
            }

            if (strcasecmp(cmd, "filter") == 0) {
                if (args == nullptr || strlen(args) == 0) {
                    NLOGW("Uso: filter <nodo1,nodo2,...>");
                } else {
                    setFilterFromList(args);
                }
            } else if (strcasecmp(cmd, "clear") == 0) {
                clearFilter();
            } else if (strcasecmp(cmd, "list") == 0) {
                printFilter();
            } else if (strcasecmp(cmd, "help") == 0) {
                printConsoleHelp();
            } else if (strlen(cmd) > 0) {
                NLOGW("Comando non riconosciuto: '%s' (digita 'help')", cmd);
            }
            continue;
        }

        // Carattere normale: accumula nel buffer di comando
        if (idx < static_cast<int>(sizeof(cmdBuf)) - 1) {
            cmdBuf[idx++] = static_cast<char>(c);
        }
        // Se il buffer e' pieno, i caratteri eccedenti vengono scartati
        // finche' non arriva un newline che resetta idx.
    }
    vTaskDelete(NULL);
}

// --- GESTORE EVENTI WIFI ---
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
    if (event_id == WIFI_EVENT_AP_START) {
        NLOGI("SoftAP avviato. SSID: %s | Canale: %d", UDP_LOG_WIFI_SSID, WIFI_CHANNEL);
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        auto* event = (wifi_event_ap_staconnected_t*) event_data;
        NLOGI("Stazione connessa - MAC: " MACSTR " | AID: %d", 
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        auto* event = (wifi_event_ap_stadisconnected_t*) event_data;
        NLOGI("Stazione disconnessa - MAC: " MACSTR " | AID: %d", 
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
            NLOGE("Impossibile creare il socket: errno %d. Riprovo...", errno);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            NLOGE("Errore bind del socket: errno %d. Riprovo...", errno);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        NLOGI("Ascolto UDP attivo sulla porta %d", UDP_LOG_PORT);

        while (1) {
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            if (len < 0) {
                NLOGE("Errore di ricezione (recvfrom): errno %d", errno);
                break; // Rompe il ciclo interno per ricreare il socket in modo pulito
            } else {
                rx_buffer[len] = 0; // Null-terminate per sicurezza stringa

                // Rimuove il newline finale (gia' presente nella riga di log
                // originale lato mittente) per evitare che NLOGI qui sotto
                // ne aggiunga un altro, producendo una riga vuota in console.
                while (len > 0 && (rx_buffer[len - 1] == '\n' || rx_buffer[len - 1] == '\r')) {
                    rx_buffer[--len] = 0;
                }

                // Il pacchetto ha il formato "<NODE_NAME>|<messaggio>".
                // Se il separatore non e' presente (es. nodo con firmware
                // vecchio), si ricade sull'IP del mittente come prima.
                const char* nodeName = nullptr;
                char* sep = strchr(rx_buffer, '|');
                char* msg = rx_buffer;
                if (sep != nullptr) {
                    *sep = 0;
                    nodeName = rx_buffer;
                    msg = sep + 1;
                }

                if (nodeName != nullptr) {
                    // Pubblica sempre verso la webpage: il filtro qui e'
                    // indipendente da quello console (nodePassesFilter) e
                    // viene applicato lato client nel browser, non qui.
                    // La formattazione avviene dentro WebLogServer, in un
                    // buffer della dimensione corretta (niente buffer
                    // intermedio sovradimensionato qui).
                    WebLogServer::pushLineFormatted(nodeName, msg);

                    if (!nodePassesFilter(nodeName)) {
                        continue; // Scarto silenzioso: nodo non nel filtro console
                    }
                    // Stampa minimale dei messaggi sul bus locale/debug
                    NLOGI("[%s]: %s", nodeName, msg);
                } else {
                    // Estrazione IP del mittente per il log (fallback)
                    char ip_str[32];
                    if (source_addr.ss_family == PF_INET) {
                        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, ip_str, sizeof(ip_str) - 1);
                    }

                    char ipPrefix[48];
                    snprintf(ipPrefix, sizeof(ipPrefix), "UDP %s:%d", ip_str, UDP_LOG_PORT);
                    WebLogServer::pushLineFormatted(ipPrefix, msg);

                    if (!nodePassesFilter(nullptr)) {
                        continue; // Scarto silenzioso: filtro console attivo, origine sconosciuta
                    }
                    NLOGI("[UDP %s:%d]: %s", ip_str, UDP_LOG_PORT, msg);
                }
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

    // Mutex per l'accesso concorrente al filtro nodi (task console + task UDP)
    s_filterMutex = xSemaphoreCreateMutex();

    // Avvio SoftAP
    wifi_init_softap();

    // Avvio web log server (pagina HTML + SSE su http://192.168.4.1/)
    WebLogServer::init();

    // Creazione del task UDP dedicato
    xTaskCreatePinnedToCore(udp_server_task, "udp_log_server", 4096, NULL, 5, NULL, tskNO_AFFINITY);

    // Creazione del task console per i comandi di filtro a runtime
    xTaskCreatePinnedToCore(console_task, "console_filter", 4096, NULL, 5, NULL, tskNO_AFFINITY);
}