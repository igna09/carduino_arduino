#include "KlineNode.h"

#include "NodeLog.h"

/** Pin TX collegato alla K-line (tramite driver LIN/ISO9141) */
static constexpr gpio_num_t TX_PIN        = GPIO_NUM_4;

/** Pin RX collegato alla K-line */
static constexpr gpio_num_t RX_PIN        = GPIO_NUM_5;

// Entry point principale con linkage C
extern "C" void app_main(void)
{
    NLOGI("Nodo Kline avviato");

    KlineNode klineNode(TX_PIN, RX_PIN);

    klineNode.delayTask(15000, [&](){
        klineNode.udp_log_sender_init();
        klineNode.enableUdpLog();
    });

    /* A questo punto app_main può continuare a fare altro, 
       come inizializzare i driver I2C o TWAI (CAN bus),
       mentre lo scheduler farà girare il task appena creato.
    */
    while (true) {
        // Loop principale vuoto o di controllo sistema
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}