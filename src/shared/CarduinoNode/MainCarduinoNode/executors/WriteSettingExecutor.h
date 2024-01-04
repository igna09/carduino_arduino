#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/SettingMessage/SettingMessage.h"
#include "../../../enums/Category.h"

class WriteSettingExecutor : public CarduinoNodeExecutorInterface {
    public:
        WriteSettingExecutor() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

        void execute(CarduinoNode *node, CanbusMessage *message) {
            SettingMessage *settingMessage = new SettingMessage(*message);
            
            if(settingMessage->setting->id == Setting::OTA_MODE.id) {
                if(settingMessage->getBoolValue()) {
                    node->otaStartup();
                } else {
                    node->otaShutdown();
                }
            }

            node->sendCanbusMessage(message);

            if(settingMessage->setting->id == Setting::RESTART.id) { // I restart after sending reset message
                delay(1000);
                if(settingMessage->getBoolValue()) {
                    node->restart();
                }
            }

            delete settingMessage;
        };
};
