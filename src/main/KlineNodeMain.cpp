#include "KlineNode.h"

static const char *TAG_KLINE_NODE = "KLINE_NODE";

/** Pin TX collegato alla K-line (tramite driver LIN/ISO9141) */
static constexpr gpio_num_t TX_PIN        = GPIO_NUM_17;

/** Pin RX collegato alla K-line */
static constexpr gpio_num_t RX_PIN        = GPIO_NUM_16;

// Entry point principale con linkage C
extern "C" void app_main(void)
{
    ESP_LOGI(TAG_KLINE_NODE, "Nodo Kline avviato");

    KlineNode klineNode(TX_PIN, RX_PIN);

    /* A questo punto app_main può continuare a fare altro, 
       come inizializzare i driver I2C o TWAI (CAN bus),
       mentre lo scheduler farà girare il task appena creato.
    */
    while (true) {
        // Loop principale vuoto o di controllo sistema
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}