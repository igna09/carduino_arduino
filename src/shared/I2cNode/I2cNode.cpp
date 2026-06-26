#include "I2cNode.h"

I2cNode::I2cNode(gpio_num_t sdaPin, gpio_num_t sclPin, uint32_t clockSpeedHz) {
    NLOGD("I2cNode::I2cNode start");
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _clockSpeedHz = clockSpeedHz;

    initI2c();

    NLOGD("I2cNode::I2cNode end");
}

void I2cNode::initI2c() {
    if (i2cInitialized) return;

    // 1. Inizializza il gestore centrale i2cdev.
    ESP_ERROR_CHECK(i2cdev_init());
    i2cInitialized = true;
    NLOGD("I2cNode: I2C initialized successfully");

    // NOTA: Rimosso initI2cDevices() da qui per evitare il crash sul metodo virtuale puro
}