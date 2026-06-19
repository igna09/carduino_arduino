#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "KLineKWP1281Lib_ESP32.h"

#include "CarduinoNode.h"
#include "KlineEcu.h"
#include "ValueToRead.h"

// ─────────────────────────────────────────────
//  Configurazione — modifica questi valori
// ─────────────────────────────────────────────

/** Indirizzo logico del modulo target (0x01 = motore, 0x17 = quadro strumenti, …) */
static constexpr uint8_t  TARGET_MODULE   = 0x01;

/** Velocità di comunicazione K-line */
static constexpr uint32_t MODULE_BAUD     = 10400;

/** Numero di porta UART da usare (UART_NUM_1 o UART_NUM_2, non UART_NUM_0 che è il monitor seriale) */
static constexpr uart_port_t KLINE_UART   = UART_NUM_1;

/** Abilitare il debug del traffico KWP1281 su console */
static constexpr bool DEBUG_TRAFFIC       = false;

// ─────────────────────────────────────────────
//  Costanti interne
// ─────────────────────────────────────────────

// static const char *TAG = "KWP_DEMO";

/** Stack del task KWP (in byte) */
static constexpr uint32_t KWP_TASK_STACK = 8192;

/** Priorità del task KWP */
static constexpr UBaseType_t KWP_TASK_PRI = 5;

/** Buffer UART HW (deve essere almeno UART_FIFO_LEN = 128) */
static constexpr int UART_BUF_SIZE = 256;

class KlineNode : public CarduinoNode {
public:
    KlineNode(gpio_num_t tx_pin, gpio_num_t rx_pin);
private:
    KLineKWP1281Lib _kline;
    uart_port_t _uart;
    gpio_num_t  _tx_pin;
    gpio_num_t  _rx_pin;
    SemaphoreHandle_t _rx_sem = nullptr;

    void uart_event_loop(QueueHandle_t uart_queue);
    void klineBegin(unsigned long baud);
    void klineEnd();
    void klineSend(uint8_t data);
    bool klineReceive(uint8_t *data, unsigned long timeout_ticks);
};
