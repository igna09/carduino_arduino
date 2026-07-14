#include "I2cNode.h"

I2cNode::I2cNode(gpio_num_t sdaPin, gpio_num_t sclPin, uint32_t clockSpeedHz) {
    NLOGD("I2cNode::I2cNode start");
    _sdaPin = sdaPin;
    _sclPin = sclPin;
    _clockSpeedHz = clockSpeedHz;

    initI2c();
    // scanI2c();

    NLOGD("I2cNode::I2cNode end");
}

void I2cNode::scanI2c() {
    NLOGD("I2cNode::scanI2C start");
    for (uint8_t addr = 0x03; addr < 0x78; addr++) {
        i2c_dev_t dev = {};
        dev.port = I2C_NUM_0;
        dev.addr = addr;
        dev.cfg.sda_io_num = DEFAULT_I2C_SDA_PIN;
        dev.cfg.scl_io_num = DEFAULT_I2C_SCL_PIN;
        dev.cfg.master.clk_speed = 100000;

        esp_err_t res = i2c_dev_probe(&dev, I2C_DEV_WRITE);
        if (res == ESP_OK) {
            NLOGI("Trovato device I2C a 0x%02X", addr);
        }
    }
    NLOGD("I2cNode::scanI2C end");
}

void I2cNode::initI2c() {
    if (i2cInitialized) return;

    // 1. Inizializza il gestore centrale i2cdev.
    ESP_ERROR_CHECK(i2cdev_init());
    i2cInitialized = true;
    NLOGD("I2cNode: I2C initialized successfully");

    // NOTA: Rimosso initI2cDevices() da qui per evitare il crash sul metodo virtuale puro
}