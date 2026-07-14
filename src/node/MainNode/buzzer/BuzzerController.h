#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/ledc.h>
#include <esp_err.h>

#include "Tone.h"

class BuzzerController {
public:
    esp_err_t init(gpio_num_t pin,
                    ledc_timer_t timer = LEDC_TIMER_0,
                    ledc_channel_t channel = LEDC_CHANNEL_0,
                    ledc_mode_t mode = LEDC_LOW_SPEED_MODE) {
        _timer = timer; _channel = channel; _mode = mode;

        ledc_timer_config_t tcfg = {};
        tcfg.speed_mode = mode;
        tcfg.timer_num = timer;
        tcfg.duty_resolution = LEDC_TIMER_10_BIT;
        tcfg.freq_hz = 440;
        tcfg.clk_cfg = LEDC_AUTO_CLK;
        esp_err_t err = ledc_timer_config(&tcfg);
        if (err != ESP_OK) return err;

        ledc_channel_config_t ccfg = {};
        ccfg.gpio_num = pin;
        ccfg.speed_mode = mode;
        ccfg.channel = channel;
        ccfg.timer_sel = timer;
        ccfg.duty = 0;
        ccfg.hpoint = 0;
        return ledc_channel_config(&ccfg);
    }

    // Non bloccante: avvia una sequenza di toni in base alla severity.
    // Ritorna ESP_ERR_INVALID_STATE se una sequenza è già in corso.
    esp_err_t playToneAsync(uint32_t freqHz, uint16_t onMs, uint16_t gapMs, uint8_t count) {
        if (_busy) return ESP_ERR_INVALID_STATE;
        _busy = true;

        auto* p = new ToneParams{this, freqHz, onMs, gapMs, count};
        BaseType_t ok = xTaskCreate(toneTask, "buzzer_seq",
                                     configMINIMAL_STACK_SIZE * 2, p, 5, NULL);
        if (ok != pdPASS) {
            _busy = false;
            delete p;
            return ESP_ERR_NO_MEM;
        }
        return ESP_OK;
    }

    esp_err_t playToneAsync(ToneType type) {
        const ToneProfile& p = getToneProfile(type);
        return playToneAsync(p.freqHz, p.onMs, p.gapMs, p.count);
    }


    bool isBusy() const { return _busy; }

private:
    struct ToneParams {
        BuzzerController* self;
        uint32_t freqHz;
        uint16_t onMs;
        uint16_t gapMs;
        uint8_t count;
    };

    ledc_timer_t _timer;
    ledc_channel_t _channel;
    ledc_mode_t _mode;
    volatile bool _busy = false;

    static void toneTask(void* arg) {
        auto* p = static_cast<ToneParams*>(arg);
        for (uint8_t i = 0; i < p->count; i++) {
            p->self->toneOn(p->freqHz);
            vTaskDelay(pdMS_TO_TICKS(p->onMs));
            p->self->toneOff();
            if (i + 1 < p->count) {
                vTaskDelay(pdMS_TO_TICKS(p->gapMs));
            }
        }
        p->self->_busy = false;
        delete p;
        vTaskDelete(NULL);
    }

    void toneOn(uint32_t freqHz) {
        ledc_set_freq(_mode, _timer, freqHz);
        ledc_set_duty(_mode, _channel, 512); // 50% duty, risoluzione 10 bit
        ledc_update_duty(_mode, _channel);
    }

    void toneOff() {
        ledc_set_duty(_mode, _channel, 0);
        ledc_update_duty(_mode, _channel);
    }
};