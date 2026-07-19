#pragma once

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <tuple>
#include <cmath>
#include <unordered_map>

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

// ─────────────────────────────────────────────
//  Configurazione — modifica questi valori
// ─────────────────────────────────────────────

/** Indirizzo logico del modulo target (0x01 = motore, 0x17 = quadro strumenti, …) */
static constexpr uint8_t  TARGET_MODULE   = 0x01;

/** Velocità di comunicazione K-line */
static constexpr uint32_t MODULE_BAUD     = 10400;

/** Numero di porta UART da usare (UART_NUM_1 o UART_NUM_2, non UART_NUM_0 che è il mnitor seriale) */
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

/** Intervallo (ms) tra due cicli di polling readValues() */
static constexpr uint32_t KLINE_POLL_INTERVAL_MS = 200;

/** Numero di tentativi di connessione falliti consecutivi prima di applicare il backoff */
static constexpr uint8_t  KLINE_MAX_CONSEC_FAILURES = 3;

/** Durata (ms) del backoff applicato a un'ECU che continua a non rispondere */
static constexpr uint32_t KLINE_BACKOFF_MS = 5000;

class KlineNode : public CarduinoNode {
public:
    KlineNode(gpio_num_t tx_pin, gpio_num_t rx_pin);

    float getLastValue(uint8_t valueToReadId) const;
private:
    /** Stato della connessione verso l'ECU corrente */
    enum class ConnState : uint8_t {
        DISCONNECTED,   // nessuna connessione attiva, va tentata
        CONNECTED,      // connessione attiva e valida
        ERROR_BACKOFF   // troppi errori consecutivi: attendere prima di ritentare
    };

    /** Stato di runtime per l'ECU correntemente connessa (o ultima tentata) */
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
    std::unordered_map<uint8_t, float> _lastValues;

    void uart_event_loop();
    void klineBegin(unsigned long baud);
    void klineEnd();
    void klineSend(uint8_t data);
    bool klineReceive(uint8_t *data, unsigned long timeout_ticks);

    // Task FreeRTOS interno dedicato al polling periodico delle ECU
    void kline_poll_loop();
    static void kline_poll_task_trampoline(void *arg);

    // Algoritmo principale di lettura: itera le ECU configurate e ne legge i blocchi richiesti
    void readValues();

    // Tenta la connessione a un'ECU se necessario, gestendo backoff/retry; ritorna true se pronti a leggere
    bool ensureConnected(KlineEcu *ecu);

    // Legge un singolo blocco (group) per l'ECU corrente e instrada i valori letti sul CAN bus.
    // Ritorna false in caso di errore di comunicazione (connessione da considerare persa).
    bool readBlock(KlineEcu *ecu, uint8_t block);

    // Instrada una misura già calcolata sul CAN bus e aggiorna lastReadValue
    void dispatchMeasurement(ValueToRead *valueToRead, float value);
};