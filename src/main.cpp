#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Il LED blu della ESP32-C3 SuperMini è solitamente sul GPIO 8
#define BLINK_GPIO GPIO_NUM_8

static const char *TAG_SYS_INFO = "SYS_INFO";
static const char *TAG_LED_INFO = "LED_INFO";

// Definizione del Task C++
void vCheckCppVersionTask(void *pvParameters)
{
    while(1) {
        ESP_LOGI(TAG_SYS_INFO, "--- Verifica ambiente di compilazione ---");
    
        // Stampa il valore numerico grezzo della macro standard
        ESP_LOGI(TAG_SYS_INFO, "Valore raw di __cplusplus: %ld", __cplusplus);

        // Identificazione dello standard ISO C++
        switch (__cplusplus) {
            case 202302L:
                ESP_LOGI(TAG_SYS_INFO, "Standard rilevato: C++23 (Modern C++)");
                break;
            case 202002L:
                ESP_LOGI(TAG_SYS_INFO, "Standard rilevato: C++20");
                break;
            case 201703L:
                ESP_LOGI(TAG_SYS_INFO, "Standard rilevato: C++17");
                break;
            case 201402L:
                ESP_LOGI(TAG_SYS_INFO, "Standard rilevato: C++14");
                break;
            case 201103L:
                ESP_LOGI(TAG_SYS_INFO, "Standard rilevato: C++11");
                break;
            default:
                if (__cplusplus > 202302L) {
                    ESP_LOGW(TAG_SYS_INFO, "Standard rilevato: Versione C++ successiva a C++23!");
                } else {
                    ESP_LOGE(TAG_SYS_INFO, "Standard rilevato: Versione C++ precedente al C++11 o non standard.");
                }
                break;
        }

        ESP_LOGI(TAG_SYS_INFO, "-----------------------------------------");

        // Pratica cruciale in FreeRTOS: un task che termina il suo scopo 
        // deve essere rimosso dallo scheduler per non causare crash.
        // ESP_LOGD(TAG_SYS_INFO, "Eliminazione del task vCheckCppVersionTask in corso...");
        // vTaskDelete(NULL); 
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Definizione del Task C++
void vBlinkLedTask(void *pvParameters)
{
    // Reset del pin e impostazione della direzione (output)
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    uint8_t led_state = 0;

    while (1) {
        // Inverte lo stato del LED
        led_state = !led_state;
        gpio_set_level(BLINK_GPIO, led_state);
        
        ESP_LOGI(TAG_LED_INFO, "LED Stato: %s", led_state ? "SPENTO" : "ACCESO");

        // Pausa di 1000 millisecondi (convertiti in Tick del FreeRTOS)
        vTaskDelay(pdMS_TO_TICKS(333));
    }
}

// Entry point principale con linkage C
extern "C" void app_main(void)
{
    ESP_LOGI(TAG_SYS_INFO, "Applicazione avviata. Creazione dei task di sistema...");

    // Creazione del task FreeRTOS
    // - Nome task: "check_cpp_task"
    // - Stack: 3072 byte (abbondante per gestire le stringhe di ESP_LOG)
    // - Priorità: 5 (priorità media)
    xTaskCreate(
        vCheckCppVersionTask,   // Funzione del task
        "check_cpp_task",       // Nome testuale (utile per il debugging)
        3072,                   // Dimensione dello Stack in byte
        NULL,                   // Parametri da passare al task (nessuno)
        5,                      // Priorità del task
        NULL                    // Handle del task (non necessario se si auto-elimina)
    );

    // Creazione del task FreeRTOS
    // - Nome task: "blink_led_task"
    // - Stack: 3072 byte (abbondante per gestire le stringhe di ESP_LOG)
    // - Priorità: 5 (priorità media)
    xTaskCreate(
        vBlinkLedTask,   // Funzione del task
        "blink_led_task",       // Nome testuale (utile per il debugging)
        3072,                   // Dimensione dello Stack in byte
        NULL,                   // Parametri da passare al task (nessuno)
        5,                      // Priorità del task
        NULL                    // Handle del task (non necessario se si auto-elimina)
    );

    /* A questo punto app_main può continuare a fare altro, 
       come inizializzare i driver I2C o TWAI (CAN bus),
       mentre lo scheduler farà girare il task appena creato.
    */
    while (true) {
        // Loop principale vuoto o di controllo sistema
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}