#pragma once

#include <stdio.h>
#include <string>
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#include "SettingBase.h"
#include "Message.h"
#include "Node.h"
#include "Definitions.h"

#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000

class CarduinoNode: public SettingBase {
public:
    bool isEnabled;

    CarduinoNode(uint8_t id);
    std::string name();
    void sendMessage(const Message& m);
    void sendByte(uint16_t messageId, int len, uint8_t *buf);

private:
    uint8_t _id;
    twai_node_handle_t _twai_node = NULL;
};
