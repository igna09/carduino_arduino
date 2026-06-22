#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <tuple>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "TEMT6000.h"
#include "aht.h"

#include "NodeLog.h"
#include "CarduinoNode.h"
#include "I2CNode.h"

// Configurazione ADC (Ad esempio usando il pin GPIO36 / ADC1 Canale 0)
#define TEMT6000_ADC_UNIT     ADC_UNIT_1
#define TEMT6000_ADC_CHANNEL  ADC_CHANNEL_0
#define TEMT6000_SAMPLES_NUM  10

class MainNode : public CarduinoNode, public I2CNode {
public:
    MainNode();
private:
    TEMT6000_t temtDevice;
    aht_t aht_dev = {0};
    float temperature, humidity;

    void configTemt6000();
};