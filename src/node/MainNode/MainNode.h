#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <tuple>
#include <map>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/queue.h>
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "aht.h"
#include "bmp280.h"
#include <esp_idf_lib_helpers.h>
#include <encoder.h>

#include "NodeLog.h"
#include "CarduinoNode.h"
#include "I2CNode.h"
#include "BootExecutor.h"
#include "HelloTrackerExecutor.h"
#include "MainNodeCanEvent.h"
#include "SwcController.h"
#include "SwcMapping.h"
#include "BuzzerController.h"
#include "Tone.h"
#include "SwcPairingEvent.h"
#include "EncoderController.h"
#include "SpeedLimitWarning.h"

// Configurazione ADC (Ad esempio usando il pin GPIO36 / ADC1 Canale 0) GPIO 0
#define TEMT6000_ADC_CHANNEL    ADC_CHANNEL_0 
#define TEMT6000_ADC_UNIT       ADC_UNIT_1

#define EV_QUEUE_LEN 5
#define GPIO_ENCODER_A  GPIO_NUM_1   // CLK
#define GPIO_ENCODER_B  GPIO_NUM_3   // DT
#define GPIO_BUTTON     GPIO_NUM_10  // SW

#define SWC_PRESS_INTERVAL  150
#define SWC_PAIRING_INTERVAL  5000
#define SWC_WAITING_PAIRING_INTERVAL  1000
#define SWC_FIRST_WAITING_PAIRING_INTERVAL  5000
#define SWC_FLAG_READY_TO_PAIR_RESET_INTERVAL 30000

enum class ClickPending { NONE, SINGLE, DOUBLE };

class MainNode : public CarduinoNode, public I2cNode {
public:
    SpeedLimitWarning _speedWarn;
    uint8_t lastSpeed = 0;

    MainNode();

    bool isEnabled = true;

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

    void handleTimeSyncRequest(uint8_t requesterId);

    static void swcPairingTask(void* param);
    volatile bool swcPairing = false;
    void startSwcPairing();


private:
    aht_t aht_dev;
    bmp280_t bpm_dev;
    float temperature, humidity;
    SwcController _swc;
    BuzzerController _buzzer;
    EncoderController _encoder;
    bool _speedLimitSetMode = false;
    static constexpr uint32_t SPEED_LIMIT_EDIT_TIMEOUT_MS = 5000;
    TimerHandle_t _speedLimitEditTimer = nullptr;

    // id nodo -> timestamp (syncedMillis()) dell'ultimo EV_HELLO ricevuto.
    // Popolata da recordHello(), a sua volta chiamato da HelloTrackerExecutor
    // ogni volta che arriva un HELLO con destination == MAIN. Utile per
    // sapere quali nodi si sono già annunciati e per rilevare nodi "persi"
    // (HELLO non più ricevuto da un certo tempo).
    std::map<uint8_t, uint32_t> _knownNodes;

    void configTemt6000();
    void initI2cDevices() override;
    void configAht();
    void configBmp();
    void configSwc();
    void configEncoder();
    void configSpeedWarning();
    void startTimeSync() override;
    uint32_t syncedMillis() const override;
    void pressSwcAsync(uint8_t channel, uint32_t holdMs);
    static void speedLimitEditTimeoutCallback(TimerHandle_t t);
    void exitSpeedLimitEditMode();
};