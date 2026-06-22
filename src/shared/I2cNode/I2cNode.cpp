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
    // Nelle versioni v5.3+, questa funzione prepara internamente i semafori (mutex) 
    // per rendere ogni successiva transazione sul bus "Thread-Safe".
    ESP_ERROR_CHECK(i2cdev_init());
    i2cInitialized = true;
    NLOGD("I2cNode: I2C initialized successfully");

    initI2cDevices();
    NLOGD("I2cNode: I2C devices initialized successfully");
}