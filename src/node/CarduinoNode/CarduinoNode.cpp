#include "CarduinoNode.h"

CarduinoNode::CarduinoNode(uint8_t id): SettingBase(), UdpLogSender() {
    _id = id;

    ESP_LOGI("CarduinoNode", "CarduinoNode::CarduinoNode start");

    // Configure TWAI node
    twai_onchip_node_config_t node_config = {
        .io_cfg = {
            .tx = DEFAULT_CAN_TX_PIN,
            .rx = DEFAULT_CAN_RX_PIN,
            .quanta_clk_out = GPIO_NUM_NC,
            .bus_off_indicator = GPIO_NUM_NC,
        },
        .bit_timing = {
            .bitrate = TWAI_BITRATE,
        },
        .fail_retry_cnt = 3,
        .tx_queue_depth = TWAI_QUEUE_DEPTH,
    };

    // Create TWAI node
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &_twai_node));

    // Register transmission completion callback
    // twai_event_callbacks_t callbacks = {
    //     .on_tx_done = twai_sender_tx_done_callback,
    //     .on_error = twai_sender_on_error_callback,
    // };
    // ESP_ERROR_CHECK(twai_node_register_event_callbacks(sender_node, &callbacks, NULL));

    // Enable TWAI node
    ESP_ERROR_CHECK(twai_node_enable(_twai_node));
    ESP_LOGI(name().c_str(), "TWAI node started successfully");

    ESP_LOGI("CarduinoNode", "CarduinoNode::CarduinoNode end");
}

std::string CarduinoNode::name() {
    return std::string(Node::getValueById(_id)->name);
}

void CarduinoNode::sendMessage(const Message& m) {
    uint16_t id = 0;
    uint8_t payload[8];
    uint8_t dlc = m.toCanFrame(id, payload, sizeof(payload));

    sendByte(m.canId(), dlc, payload);
}

void CarduinoNode::sendByte(uint16_t messageId, int len, uint8_t *buf) {
    twai_frame_t tx_frame = {
        .header = {
            .id = messageId,
        },
        .buffer = buf,
        .buffer_len = static_cast<size_t>(len),
    };

    ESP_ERROR_CHECK(twai_node_transmit(_twai_node, &tx_frame, 500));
};

void CarduinoNode::delayTask(unsigned long millisec, std::function<void()> lambda) {
    // Passiamo i ms e la lambda al task FreeRTOS
    struct TaskArgs {
        unsigned long millisec;
        std::function<void()> lambda;
    };
    
    auto *args = new TaskArgs{millisec, lambda};

    xTaskCreate(
        [](void *param) {
            auto *p = static_cast<TaskArgs *>(param);
            
            // Attende il tempo richiesto senza bloccare l'ESP32
            vTaskDelay(pdMS_TO_TICKS(p->millisec));
            
            // Esegue la lambda
            p->lambda();
            
            // Pulisce la memoria ed elimina il task autonomamente
            delete p;
            vTaskDelete(NULL); 
        },
        "delayed_lambda",
        3072, // Stack size
        args,
        1,    // Priorità bassa
        nullptr
    );
}
