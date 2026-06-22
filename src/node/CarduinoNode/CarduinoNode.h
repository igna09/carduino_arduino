#pragma once

#include <stdio.h>
#include <string>
#include <functional>
#include <map>
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#include "SettingBase.h"
#include "Message.h"
#include "Node.h"
#include "Definitions.h"
#include "UdpLogSender.h"

#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000

class CarduinoNode: public SettingBase, public UdpLogSender {
public:
    bool isEnabled;

    CarduinoNode(uint8_t id);
    std::string name();
    void sendMessage(const Message& m);
    void sendByte(uint16_t messageId, int len, uint8_t *buf);
    void delayTask(unsigned long millisec, std::function<void()> lambda);
    void startRepeatingTask(const std::string& id, unsigned long millisec, std::function<void()> fn, uint32_t stackSize = 4096, UBaseType_t priority = 5);
    void stopRepeatingTask(const std::string& id);
    void stopAllRepeatingTasks();

private:
    uint8_t _id;
    twai_node_handle_t _twai_node = NULL;
    struct RepeatingTaskCtx {
        std::function<void()> fn;
        uint32_t periodMs;
        std::string id;
        volatile bool stop;
    };
    struct TaskEntry {
        TaskHandle_t handle;
        RepeatingTaskCtx* ctx;
    };
    std::map<std::string, TaskEntry> tasks_;
};
