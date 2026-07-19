#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <tuple>
#include <cmath>
#include <unordered_map>
#include <variant>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "KLineKWP1281Lib_ESP32.h"
#include "NodeLog.h"
#include "CarduinoNode.h"
#include "KlineEcu.h"
#include "ValueToRead.h"
#include "AfterReadExecutors.h"
#include "FuelConsumptionExecutor.h"
#include "CruiseExecutor.h"

static constexpr uart_port_t KLINE_UART   = UART_NUM_1;
static constexpr uint32_t KWP_TASK_STACK = 8192;
static constexpr UBaseType_t KWP_TASK_PRI = 5;
static constexpr int UART_BUF_SIZE = 256;
static constexpr uint32_t KLINE_POLL_INTERVAL_MS = 200;
static constexpr uint8_t   KLINE_MAX_CONSEC_FAILURES = 3;
static constexpr uint32_t KLINE_BACKOFF_MS = 5000;

class KlineNode : public CarduinoNode {
public:
    KlineNode(gpio_num_t tx_pin, gpio_num_t rx_pin);

    template <typename T>
    T getLastValue(uint8_t valueToReadId) const {
        auto it = _lastValues.find(valueToReadId);
        if (it != _lastValues.end()) {
            // std::visit prende il valore dentro il variant (qualsiasi esso sia)
            // e lo converte in modo sicuro nel tipo T richiesto
            return std::visit([](auto&& arg) -> T {
                return static_cast<T>(arg);
            }, it->second);
        }
        return T{}; 
    }

private:
    enum class ConnState : uint8_t { DISCONNECTED, CONNECTED, ERROR_BACKOFF };

    struct EcuRuntimeState {
        KlineEcu  *ecu               = nullptr;
        ConnState  connState         = ConnState::DISCONNECTED;
        uint8_t    consecFails       = 0;
        TickType_t backoffUntilTicks = 0;
    };

    KLineKWP1281Lib _kline;
    uart_port_t _uart = KLINE_UART;
    gpio_num_t  _tx_pin;
    gpio_num_t  _rx_pin;
    SemaphoreHandle_t _rx_sem = nullptr;
    EcuRuntimeState _currentEcu;
    AfterReadExecutors _afterReadExecutors;
    QueueHandle_t _uart_queue;
    TaskHandle_t _uart_task_handle;

    std::unordered_map<uint8_t, std::variant<int, float, bool>> _lastValues;

    void uart_event_loop();
    void klineBegin(unsigned long baud);
    void klineEnd();
    void klineSend(uint8_t data);
    bool klineReceive(uint8_t *data, unsigned long timeout_ticks);
    void kline_poll_loop();
    static void kline_poll_task_trampoline(void *arg);
    void readValues();
    bool ensureConnected(KlineEcu *ecu);
    bool readBlock(KlineEcu *ecu, uint8_t block);
    void dispatchMeasurement(ValueToRead *valueToRead, float value); 
};