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
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "aht.h"

#include "NodeLog.h"
#include "CarduinoNode.h"
#include "I2CNode.h"
#include "BootExecutor.h"

// Configurazione ADC (Ad esempio usando il pin GPIO36 / ADC1 Canale 0)
#define TEMT6000_ADC_CHANNEL    ADC_CHANNEL_0 
#define TEMT6000_ADC_UNIT       ADC_UNIT_1

class MainNode : public CarduinoNode, public I2cNode {
public:
    MainNode();

    void enable() override;
private:
    aht_t aht_dev;
    float temperature, humidity;

    void configTemt6000();
    void initI2cDevices() override;
    void configAht();
};