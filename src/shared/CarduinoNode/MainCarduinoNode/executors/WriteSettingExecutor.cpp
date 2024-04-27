#include "WriteSettingExecutor.h"

WriteSettingExecutor::WriteSettingExecutor() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void WriteSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    SettingMessage *settingMessage = new SettingMessage(*message);
    
    if(settingMessage->setting->id == Setting::OTA_MODE.id) {
        if(settingMessage->getBoolValue()) {
            node->otaStartup();
        } else {
            node->otaShutdown();
        }
    } else if(settingMessage->setting->id == Setting::SWC_PAIR.id) { //TODO: move to new node in canbus executor
        if(settingMessage->getBoolValue()) {
            ((MainCarduinoNode*)node)->startSwcPairing();
        }
    }

    node->sendCanbusMessage(message);

    if(settingMessage->setting->id == Setting::RESTART.id) { // I restart after sending reset message
        if(settingMessage->getBoolValue()) {
            delay(1000);
            node->restart();
        }
    }

    delete settingMessage;
};
