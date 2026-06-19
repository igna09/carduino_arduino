#include "KlineNode.h"

KlineNode::KlineNode(gpio_num_t tx_pin, gpio_num_t rx_pin): CarduinoNode(0x01) {
    _tx_pin = tx_pin;
    _rx_pin = rx_pin;

    _kline = KLineKWP1281Lib{
        [this](unsigned long baud)                          { klineBegin(baud); },
        [this]()                                            { klineEnd(); },
        [this](uint8_t data)                                { klineSend(data); },
        [this](uint8_t *data, unsigned long timeout_ticks)  { return klineReceive(data, timeout_ticks); },
        static_cast<uint8_t>(tx_pin)   // pin TX per il bit-bang 5-baud
    };

    _rx_sem = xSemaphoreCreateBinary();
    configASSERT(_rx_sem);
}


void KlineNode::uart_event_loop(QueueHandle_t uart_queue) {
    uart_event_t event;
    while (true) {
        if (xQueueReceive(uart_queue, &event, portMAX_DELAY)) {
            if (event.type == UART_DATA || event.type == UART_BUFFER_FULL) {
                xSemaphoreGive(_rx_sem);
            }
        }
    }
}

void KlineNode::klineBegin(unsigned long baud) {
    uart_driver_delete(_uart);

    const uart_config_t uart_cfg = {
        .baud_rate  = static_cast<int>(baud),
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags      = {}
    };

    QueueHandle_t uart_queue;
    ESP_ERROR_CHECK(uart_param_config(_uart, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(_uart, _tx_pin, _rx_pin,
                                    UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(_uart, UART_BUF_SIZE, 0,
                                            10, &uart_queue, 0));

    // Soglia FIFO a 1 byte: l'interrupt scatta appena arriva il primo byte.
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(_uart, 1));

    xSemaphoreTake(_rx_sem, 0);

    // Il task evento viene creato come lambda non-capturing (richiesto
    // da xTaskCreate, che accetta solo funzioni "C-style"), passando
    // `this` e la coda UART tramite il parametro void* arg.
    struct TaskArgs { KlineNode *self; QueueHandle_t queue; };
    auto *task_args = new TaskArgs{this, uart_queue};

    xTaskCreate(
        [](void *arg) {
            auto *args = static_cast<TaskArgs *>(arg);
            KlineNode *self = args->self;
            QueueHandle_t queue = args->queue;
            delete args;
            self->uart_event_loop(queue);
        },
        "uart_evt", 2048, task_args, KWP_TASK_PRI + 1, nullptr
    );
};

void KlineNode::klineEnd() {
    uart_driver_delete(_uart);
};

void KlineNode::klineSend(uint8_t data) {
    uart_write_bytes(_uart, reinterpret_cast<const char *>(&data), 1);
};

bool KlineNode::klineReceive(uint8_t *data, unsigned long timeout_ticks) {
    int bytes_read = uart_read_bytes(_uart, data, 1, 0);
    if (bytes_read == 1) {
        return true;
    }

    if (xSemaphoreTake(_rx_sem, static_cast<TickType_t>(timeout_ticks)) != pdTRUE) {
        return false; // timeout
    }

    bytes_read = uart_read_bytes(_uart, data, 1, 0);
    return bytes_read == 1;
};
