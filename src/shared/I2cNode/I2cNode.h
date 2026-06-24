#pragma once

#include "driver/gpio.h"
#include "i2cdev.h"

#include "Definitions.h"
#include "NodeLog.h"

class I2cNode {
public:
    bool i2cInitialized = false;

    I2cNode(gpio_num_t sdaPin = DEFAULT_I2C_SDA_PIN, gpio_num_t sclPin = DEFAULT_I2C_SCL_PIN, uint32_t clockSpeedHz = DEFAULT_I2C_CLOCK_SPEED);
    virtual void initI2cDevices() = 0;
private:
    gpio_num_t _sdaPin;
    gpio_num_t _sclPin;
    uint32_t _clockSpeedHz;

    void initI2c();
};