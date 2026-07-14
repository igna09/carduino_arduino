#pragma once

#include <pcf8574.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_err.h>
#include <string.h>

class SwcController;

struct PressParams { SwcController* ctrl; uint8_t channel; uint32_t holdMs; };

class SwcController {
public:
    esp_err_t init(i2c_port_t port, uint8_t addr, gpio_num_t sda, gpio_num_t scl) {
        memset(&_dev, 0, sizeof(i2c_dev_t));
        esp_err_t err = pcf8574_init_desc(&_dev, addr, port, sda, scl);
        if (err != ESP_OK) return err;

        _shadow = 0x00;                          // idle: tutte le uscite basse
        _i2cMutex = xSemaphoreCreateMutex();
        _busy = false;
        return pcf8574_port_write(&_dev, _shadow);
    }

    // Non bloccante: lancia un task che tiene il canale a GND per holdMs.
    // Ritorna ESP_ERR_INVALID_STATE se un'altra pressione è già in corso.
    esp_err_t pressOneShotAsync(uint8_t channel, uint32_t holdMs) {
        if (channel > 7) return ESP_ERR_INVALID_ARG;
        if (_busy) return ESP_ERR_INVALID_STATE;

        _busy = true;
        auto* p = new PressParams{this, channel, holdMs};
        BaseType_t ok = xTaskCreate(pressTask, "swc_press",
                                     configMINIMAL_STACK_SIZE * 2, p, 5, NULL);
        if (ok != pdPASS) {
            _busy = false;
            delete p;
            return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }

    bool isBusy() const { return _busy; }

private:
    struct PressParams { SwcController* self; uint8_t channel; uint32_t holdMs; };

    i2c_dev_t _dev;
    uint8_t _shadow;
    SemaphoreHandle_t _i2cMutex;
    volatile bool _busy;

    static void pressTask(void* arg) {
        auto* p = static_cast<PressParams*>(arg);
        p->self->doPress(p->channel, p->holdMs);
        delete p;
        vTaskDelete(NULL);
    }

    void doPress(uint8_t channel, uint32_t holdMs) {
        xSemaphoreTake(_i2cMutex, portMAX_DELAY);

        uint8_t on = _shadow | (1 << channel);   // attiva: bit alto
        pcf8574_port_write(&_dev, on);

        xSemaphoreGive(_i2cMutex);
        vTaskDelay(pdMS_TO_TICKS(holdMs));
        xSemaphoreTake(_i2cMutex, portMAX_DELAY);

        _shadow &= ~(1 << channel);              // rilascio: torna basso
        pcf8574_port_write(&_dev, _shadow);

        xSemaphoreGive(_i2cMutex);
        _busy = false;
    }
};