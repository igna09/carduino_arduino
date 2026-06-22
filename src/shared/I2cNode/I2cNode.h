#pragma once

#include "driver/gpio.h"
#include "i2cdev.h"

#include "Definitions.h"

class I2cNode {
public:
    bool i2cInitialized = false;

    I2cNode(gpio_num_t sdaPin = DEFAULT_I2C_SDA_PIN, gpio_num_t sclPin = DEFAULT_I2C_SCL_PIN, uint32_t clockSpeedHz = DEFAULT_I2C_CLOCK_SPEED);
    virtual void initI2cDevices() = 0;
private:
    void initI2c();
};