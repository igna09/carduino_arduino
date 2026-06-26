#include <stdio.h>
#include <atomic>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_random.h"

#define TX_GPIO_NUM       GPIO_NUM_4
#define RX_GPIO_NUM       GPIO_NUM_5
#define TAG               "CAN_TEST"
#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000
// Buffer for burst data handling
#define POLL_DEPTH              200
#define BURST_SIZE 300
// Recovery: tempo massimo di attesa dopo twai_node_recover() prima di considerarla fallita
#define RECOVERY_WAIT_MS        1000

twai_frame_t tx_frame;

// Flag aggiornato dal callback ISR di state-change: true quando il bus è in bus_off.
// std::atomic<bool> garantisce la visibilità tra ISR e task senza bisogno di mutex.
static std::atomic<bool> s_bus_off{false};

typedef struct {
    twai_frame_t frame;
    uint8_t data[TWAI_FRAME_MAX_LEN];
} twai_listener_data_t;

typedef struct {
    twai_node_handle_t node_hdl;
    twai_listener_data_t *rx_pool;
    SemaphoreHandle_t free_pool_semaphore;
    SemaphoreHandle_t rx_result_semaphore;
    int write_idx;
    int read_idx;
} twai_listener_ctx_t;


twai_listener_ctx_t twai_listener_ctx = {0};

// Error callback
static bool IRAM_ATTR twai_listener_on_error_callback(twai_node_handle_t handle, const twai_error_event_data_t *edata, void *user_ctx)
{
    ESP_EARLY_LOGW(TAG, "bus error: 0x%x", edata->err_flags.val);
    return false;
}

// Node state
static bool IRAM_ATTR twai_listener_on_state_change_callback(twai_node_handle_t handle, const twai_state_change_event_data_t *edata, void *user_ctx)
{
    const char *twai_state_name[] = {"error_active", "error_warning", "error_passive", "bus_off"};
    ESP_EARLY_LOGI(TAG, "state changed: %s -> %s", twai_state_name[edata->old_sta], twai_state_name[edata->new_sta]);

    // Aggiorniamo solo il flag atomico: siamo in contesto ISR, nessuna chiamata bloccante
    // o di recovery va fatta qui. La gestione effettiva avviene nel task TX.
    if (edata->new_sta == TWAI_ERROR_BUS_OFF) {
        s_bus_off.store(true, std::memory_order_relaxed);
    } else if (edata->new_sta == TWAI_ERROR_ACTIVE) {
        s_bus_off.store(false, std::memory_order_relaxed);
    }
    return false;
}

// TWAI receive callback - store data and signal
static bool IRAM_ATTR twai_listener_rx_callback(twai_node_handle_t handle, const twai_rx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t woken;
    twai_listener_ctx_t *ctx = (twai_listener_ctx_t *)user_ctx;

    if (xSemaphoreTakeFromISR(ctx->free_pool_semaphore, &woken) != pdTRUE) {
        ESP_EARLY_LOGI(TAG, "Pool full, dropping frame");
        return (woken == pdTRUE);
    }
    if (twai_node_receive_from_isr(handle, &ctx->rx_pool[ctx->write_idx].frame) == ESP_OK) {
        ctx->write_idx = (ctx->write_idx + 1) % POLL_DEPTH;
        xSemaphoreGiveFromISR(ctx->rx_result_semaphore, &woken);
    }
    return (woken == pdTRUE);
}

void setupCanbus() {
    twai_onchip_node_config_t node_config = {
        .io_cfg = {
            .tx = TX_GPIO_NUM,
            .rx = RX_GPIO_NUM,
            .quanta_clk_out = GPIO_NUM_NC,
            .bus_off_indicator = GPIO_NUM_NC,
        },
        .bit_timing = {
            .bitrate = TWAI_BITRATE,
        },
        .timestamp_resolution_hz = 1000000, // 1 MHz = Risoluzione di 1 microsecondo
        .fail_retry_cnt = 3,
        .tx_queue_depth = TWAI_QUEUE_DEPTH,
        .flags = {
            .enable_self_test = 1,
            .enable_loopback = 1,
        },
    };

    tx_frame = {
        .header = {
            .id = 0x10,
        },
        .buffer = {},
        .buffer_len = static_cast<size_t>(0),
    };

    twai_listener_ctx = {0};
    twai_listener_ctx.free_pool_semaphore = xSemaphoreCreateCounting(POLL_DEPTH, POLL_DEPTH);
    twai_listener_ctx.rx_result_semaphore = xSemaphoreCreateCounting(POLL_DEPTH, 0);
    assert(twai_listener_ctx.free_pool_semaphore != NULL);
    assert(twai_listener_ctx.rx_result_semaphore != NULL);

    // Create TWAI node
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &twai_listener_ctx.node_hdl));;

    twai_listener_ctx.rx_pool = static_cast<twai_listener_data_t*>(calloc(POLL_DEPTH, sizeof(twai_listener_data_t)));
    assert(twai_listener_ctx.rx_pool != NULL);
    for (int i = 0; i < POLL_DEPTH; i++) {
        twai_listener_ctx.rx_pool[i].frame.buffer = twai_listener_ctx.rx_pool[i].data;
        twai_listener_ctx.rx_pool[i].frame.buffer_len = sizeof(twai_listener_ctx.rx_pool[i].data);
    }
    ESP_LOGI(TAG, "Buffer initialized: %d slots for burst data", POLL_DEPTH);

    twai_event_callbacks_t callbacks = {
        .on_rx_done = twai_listener_rx_callback,
        .on_state_change = twai_listener_on_state_change_callback,
        .on_error = twai_listener_on_error_callback,
    };
    ESP_ERROR_CHECK(twai_node_register_event_callbacks(twai_listener_ctx.node_hdl, &callbacks, &twai_listener_ctx));

    // Enable TWAI node
    ESP_ERROR_CHECK(twai_node_enable(twai_listener_ctx.node_hdl));
    ESP_LOGI(TAG, "TWAI start listening...");
}

// Controlla lo stato del bus e, se siamo in bus_off, avvia la recovery e attende
// che il nodo torni in error_active prima di permettere nuove trasmissioni.
// Ritorna true se il bus è (o torna) operativo, false se la recovery è ancora in corso/fallita.
static bool recoverCanbusIfNeeded() {
    if (!s_bus_off.load(std::memory_order_relaxed)) {
        return true; // tutto ok, nessuna recovery necessaria
    }

    ESP_LOGW(TAG, "Bus in BUS_OFF, avvio recovery...");
    esp_err_t err = twai_node_recover(twai_listener_ctx.node_hdl);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "twai_node_recover() fallita: %s", esp_err_to_name(err));
        return false;
    }

    // Attendiamo il ritorno a error_active, controllando periodicamente lo stato reale del nodo
    // (oltre al flag, così siamo robusti anche se l'evento on_state_change fosse perso).
    const int step_ms = 50;
    int waited_ms = 0;
    twai_node_status_t node_status;
    while (waited_ms < RECOVERY_WAIT_MS) {
        vTaskDelay(pdMS_TO_TICKS(step_ms));
        waited_ms += step_ms;

        if (twai_node_get_info(twai_listener_ctx.node_hdl, &node_status, NULL) == ESP_OK &&
            node_status.state == TWAI_ERROR_ACTIVE) {
            s_bus_off.store(false, std::memory_order_relaxed);
            ESP_LOGI(TAG, "Recovery completata, nodo error_active dopo %d ms", waited_ms);
            return true;
        }
    }

    ESP_LOGE(TAG, "Recovery non completata dopo %d ms, nodo ancora in errore", RECOVERY_WAIT_MS);
    return false;
}

static void txMessage(void *pvParameters) {
    while (1) {
        // Prima di ogni burst verifichiamo lo stato del bus: se è in bus_off avviamo
        // la procedura di recovery e, se non si conclude in tempo, saltiamo il burst.
        if (!recoverCanbusIfNeeded()) {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        ESP_LOGI(TAG, "--- Inizio Burst di %d messaggi ---", BURST_SIZE);

        // Creiamo un array di frame e un array di payload dedicati per l'intero burst.
        // Essendo allocati a inizio ciclo (o statici), rimangono validi e isolati 
        // per tutta la durata delle trasmissioni.
        static twai_frame_t local_frames[BURST_SIZE];
        static uint8_t tx_data_buffers[BURST_SIZE][8];

        memset(local_frames, 0, sizeof(local_frames));
        memset(tx_data_buffers, 0, sizeof(tx_data_buffers));

        for (int i = 0; i < BURST_SIZE; i++) {
            // Se durante il burst il bus va in bus_off, interrompiamo subito:
            // la recovery verrà gestita al prossimo giro del ciclo esterno.
            if (s_bus_off.load(std::memory_order_relaxed)) {
                ESP_LOGW(TAG, "Bus_off rilevato durante il burst, interrompo trasmissione corrente");
                break;
            }

            // Genera una lunghezza random del payload (DLC) tra 0 e 8
            size_t random_len = esp_random() % 9; 

            // Riempi il buffer i-esimo con byte casuali
            for (size_t j = 0; j < random_len; j++) {
                tx_data_buffers[i][j] = static_cast<uint8_t>(esp_random() % 256);
            }

            // Configura i parametri del frame i-esimo puntando al buffer i-esimo
            local_frames[i].header.id = 0x10;
            local_frames[i].header.dlc = random_len;
            local_frames[i].buffer = tx_data_buffers[i]; // <--- Ogni frame ha il suo spazio isolato!
            local_frames[i].buffer_len = random_len;

            // Trasmissione del messaggio
            esp_err_t tx_err = twai_node_transmit(twai_listener_ctx.node_hdl, &local_frames[i], pdMS_TO_TICKS(50));
            
            if (tx_err != ESP_OK) {
                if (tx_err == ESP_ERR_TIMEOUT) {
                    ESP_LOGE(TAG, "[%d] Timeout trasmissione", i);
                    break;
                }
            }

            // Mantenuto a 1 ms come richiesto
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        ESP_LOGI(TAG, "--- Fine Burst, attesa di 2 secondi ---");
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void rxMessage(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(twai_listener_ctx.rx_result_semaphore, portMAX_DELAY) == pdTRUE) {
            // Salviamo l'indice corrente in una variabile locale prima che venga incrementato
            int current_idx = twai_listener_ctx.read_idx;
            twai_frame_t *frame = &twai_listener_ctx.rx_pool[current_idx].frame;
            
            // Buffer temporaneo per il payload in esadecimale
            char payload_str[25] = {0}; 
            int offset = 0;
            
            // Cicla solo per i byte effettivi del DLC
            for (int i = 0; i < frame->header.dlc; i++) {
                offset += snprintf(payload_str + offset, sizeof(payload_str) - offset, "%x ", frame->buffer[i]);
            }

            // Aggiunto l'indice [Buf:%d] all'inizio del messaggio di log
            ESP_LOGI(TAG, "RX [Buf:%d]: timestamp %llu, %lx [%d] %s", \
                     current_idx, \
                     frame->header.timestamp, \
                     (long unsigned int)frame->header.id, \
                     frame->header.dlc, \
                     payload_str);
                     
            // Incremento dell'indice circolare
            twai_listener_ctx.read_idx = (twai_listener_ctx.read_idx + 1) % POLL_DEPTH;
            xSemaphoreGive(twai_listener_ctx.free_pool_semaphore);
        }
    }
}

extern "C" void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    setupCanbus();

    xTaskCreate(txMessage, "SEND_TASK", 8192, NULL, 5, NULL);
    xTaskCreate(rxMessage, "RECEIVE_TASK", 4096, NULL, 6, NULL);
}