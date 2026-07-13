#include "MainNode.h"

#include "NodeLog.h"

// Entry point principale con linkage C
extern "C" void app_main(void)
{
    vTaskDelay(pdMS_TO_TICKS(5000));

    NLOGI("Nodo Main avviato");

    MainNode *mainNode = new MainNode();

    // mainNode->delayTask(10000, [mainNode](){
    //     // mainNode.enableUdpLog();
    //     mainNode->enable();
    // });

    /* A questo punto app_main può continuare a fare altro, 
       come inizializzare i driver I2C o TWAI (CAN bus),
       mentre lo scheduler farà girare il task appena creato.
    */
    while (true) {
        // Loop principale vuoto o di controllo sistema
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}