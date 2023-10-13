#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/SettingMessage/SettingMessage.h"
#include "shared/enums/Category.h"

class WriteOtaSetting : public CarduinoNodeExecutorInterface {
    public:
        WriteOtaSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

        void execute(CarduinoNode *node, CanbusMessage *message) {
            Serial.println("received setting message");
            SettingMessage *settingMessage = new SettingMessage(*message);
            
            if(settingMessage->setting->id == Setting::OTA_MODE.id) {
                if(settingMessage->getBoolValue()) {
                    node->otaStartup();
                } else {
                    node->otaShutdown();
                }
            }

            delete settingMessage;
        };
};
