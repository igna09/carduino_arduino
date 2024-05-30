#include "CarduinoNodeCanGetSettings.h"

CarduinoNodeCanGetSettings::CarduinoNodeCanGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void CarduinoNodeCanGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    std::map<uint8_t, SettingInformation*>::iterator it;
    
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        /**
         * TODO: add a flag to check if i'm the main node?
        */
        if(it->first != Setting::OTA_MODE.id && it->first != Setting::RESTART.id) { // these are managed from main node
            SettingInformation *settingInformation = it->second;
            Setting *setting = (Setting*) Setting::getValueById(it->first);

            ReadSettingMessage *readSettingMessage = nullptr;
            if(setting->type->id == CanbusMessageType::INT.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->intValue);
            } else if (setting->type->id == CanbusMessageType::FLOAT.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->floatValue);
            } else if (setting->type->id == CanbusMessageType::BOOL.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->boolValue);
            }
            
            node->sendCanbusMessage(readSettingMessage);
            delete readSettingMessage;
        }
    }
};
