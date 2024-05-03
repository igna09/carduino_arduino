#include "CarduinoNodeWriteSetting.h"

CarduinoNodeWriteSetting::CarduinoNodeWriteSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void CarduinoNodeWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    WriteSettingMessage *settingMessage = new WriteSettingMessage(*message);

    if(settingMessage->setting->id == Setting::OTA_MODE.id) {
        if(settingMessage->getBoolValue()) {
            node->otaStartup();
        } else {
            node->otaShutdown();
        }
    } else if(settingMessage->setting->id == Setting::RESTART.id) {
        if(settingMessage->getBoolValue()) {
            node->restart();
        }
    }

    delete settingMessage;
};
