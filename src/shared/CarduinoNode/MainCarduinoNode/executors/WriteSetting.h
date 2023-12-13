#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/SettingMessage/SettingMessage.h"
#include "../../../enums/Category.h"

class WriteSetting : public CarduinoNodeExecutorInterface {
    public:
        WriteSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

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

            delete settingMessage;
        };
};
