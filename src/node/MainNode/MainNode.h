#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <tuple>
#include <map>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "aht.h"

#include "NodeLog.h"
#include "CarduinoNode.h"
#include "I2CNode.h"
#include "BootExecutor.h"
#include "HelloTrackerExecutor.h"

// Configurazione ADC (Ad esempio usando il pin GPIO36 / ADC1 Canale 0)
#define TEMT6000_ADC_CHANNEL    ADC_CHANNEL_0 
#define TEMT6000_ADC_UNIT       ADC_UNIT_1

class MainNode : public CarduinoNode, public I2cNode {
public:
    MainNode();

    void enable() override;

    // Chiamato da HelloTrackerExecutor quando arriva un EV_HELLO da
    // senderId: aggiorna (o crea) l'entry in _knownNodes con il timestamp
    // corrente. Pubblico per lo stesso motivo per cui handleTimeSyncRequest
    // è pubblico in CarduinoNode: chiamato dall'executor, non da MainNode
    // stesso.
    void recordHello(uint8_t senderId);

    // True se è arrivato almeno un HELLO da nodeId da quando MAIN è partito.
    bool hasSeenNode(uint8_t nodeId) const;

    // Timestamp (syncedMillis()) dell'ultimo HELLO ricevuto da nodeId, o 0
    // se non ne è mai arrivato uno. Usare hasSeenNode() per distinguere "non
    // visto" da "visto a syncedMillis()==0".
    uint32_t lastHelloMillis(uint8_t nodeId) const;

private:
    aht_t aht_dev;
    float temperature, humidity;

    // id nodo -> timestamp (syncedMillis()) dell'ultimo EV_HELLO ricevuto.
    // Popolata da recordHello(), a sua volta chiamato da HelloTrackerExecutor
    // ogni volta che arriva un HELLO con destination == MAIN. Utile per
    // sapere quali nodi si sono già annunciati e per rilevare nodi "persi"
    // (HELLO non più ricevuto da un certo tempo).
    std::map<uint8_t, uint32_t> _knownNodes;

    void configTemt6000();
    void initI2cDevices() override;
    void configAht();
};