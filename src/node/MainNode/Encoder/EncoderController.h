#pragma once
#include <functional>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/timers.h>
#include <encoder.h>
#include "NodeLog.h"

#define ENC_EV_QUEUE_LEN 5

class EncoderController {
public:
    using ClickCb   = std::function<void()>;
    using RotateCb  = std::function<void(int32_t diff)>;

    void setOnSingleClick(ClickCb cb)  { _onSingle = std::move(cb); }
    void setOnDoubleClick(ClickCb cb)  { _onDouble = std::move(cb); }
    void setOnTripleClick(ClickCb cb)  { _onTriple = std::move(cb); }
    void setOnLongPress(ClickCb cb)    { _onLongPress = std::move(cb); }
    void setOnRotate(RotateCb cb)      { _onRotate = std::move(cb); }
    void setOnRotateHeld(RotateCb cb)  { _onRotateHeld = std::move(cb); }

    esp_err_t init(gpio_num_t pinA, gpio_num_t pinB, gpio_num_t pinBtn) {
        _event_queue = xQueueCreate(ENC_EV_QUEUE_LEN, sizeof(rotary_encoder_event_t));

        rotary_encoder_config_t config = ROTARY_ENCODER_DEFAULT_CONFIG();
        config.pin_a = pinA;
        config.pin_b = pinB;
        config.pin_btn = pinBtn;
        config.callback = &EncoderController::encoderEventHandler;
        config.callback_ctx = _event_queue;

        esp_err_t err = rotary_encoder_create(&config, &_re);
        if (err != ESP_OK) return err;

        xTaskCreate(&EncoderController::encoderTask, "encoder_task",
                    configMINIMAL_STACK_SIZE * 8, this, 5, NULL);

        _clickTimer = xTimerCreate("clickTmr", pdMS_TO_TICKS(MULTI_CLICK_WINDOW_MS),
                                    pdFALSE, this, &EncoderController::clickTimerCallback);
        if (_clickTimer == nullptr) {
            NLOGE("Encoder xTimerCreate FAILED - click timer disabled");
            return ESP_FAIL;
        }
        return ESP_OK;
    }

private:
    static constexpr uint32_t MULTI_CLICK_WINDOW_MS = 300;

    QueueHandle_t _event_queue = nullptr;
    rotary_encoder_handle_t _re = nullptr;
    TimerHandle_t _clickTimer = nullptr;

    bool _btnHeld = false;
    bool _longPressFired = false;
    bool _rotatedWhileHeld = false;
    uint8_t _clickCount = 0;

    ClickCb _onSingle, _onDouble, _onTriple, _onLongPress;
    RotateCb _onRotate, _onRotateHeld;

    static void encoderEventHandler(const rotary_encoder_event_t *event, void *ctx) {
        QueueHandle_t q = static_cast<QueueHandle_t>(ctx);
        xQueueSend(q, event, 0);
    }

    static void clickTimerCallback(TimerHandle_t t) {
        auto* self = static_cast<EncoderController*>(pvTimerGetTimerID(t));
        self->flushClicks();
    }

    static void encoderTask(void *arg) {
        auto* self = static_cast<EncoderController*>(arg);
        rotary_encoder_event_t e;
        while (1) {
            xQueueReceive(self->_event_queue, &e, portMAX_DELAY);
            switch (e.type) {
                case RE_ET_BTN_PRESSED:
                    self->_rotatedWhileHeld = false;
                    self->_btnHeld = true;
                    break;
                case RE_ET_BTN_RELEASED:
                    self->onButtonReleased();
                    break;
                case RE_ET_CHANGED:
                    self->onRotation(e.diff);
                    break;
                case RE_ET_BTN_LONG_PRESSED:
                    if (!self->_rotatedWhileHeld) {
                        self->_longPressFired = true;
                        if (self->_onLongPress) self->_onLongPress();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    void onButtonReleased() {
        _btnHeld = false;
        if (_longPressFired) { _longPressFired = false; return; }
        if (_rotatedWhileHeld) return;

        _clickCount++;
        if (_clickTimer == nullptr) return;
        xTimerStop(_clickTimer, 0);
        xTimerStart(_clickTimer, 0);
    }

    void onRotation(int32_t diff) {
        if (_btnHeld) {
            _rotatedWhileHeld = true;
            if (_onRotateHeld) _onRotateHeld(diff);
        } else {
            if (_onRotate) _onRotate(diff);
        }
    }

    void flushClicks() {
        switch (_clickCount) {
            case 1: if (_onSingle) _onSingle(); break;
            case 2: if (_onDouble) _onDouble(); break;
            default: if (_clickCount >= 3 && _onTriple) _onTriple(); break;
        }
        _clickCount = 0;
    }
};