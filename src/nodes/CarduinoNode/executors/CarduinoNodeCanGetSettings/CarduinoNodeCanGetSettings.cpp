#include "CarduinoNodeCanGetSettings.h"

CarduinoNodeCanGetSettings::CarduinoNodeCanGetSettings() : CarduinoNodeExecutorInterface(&EventEnum::GET_SETTINGS) {};

void CarduinoNodeCanGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    std::map<uint8_t, SettingInformation*>::iterator it;
    
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        /**
         * TODO: add a flag to check if i'm the main node?
        */
        if(it->first != Setting::OTA_MODE.id) { // this is managed from main node
            SettingInformation *settingInformation = it->second;
            Setting *setting = (Setting*) Setting::getValueById(it->first);

            CanbusMessage *settingMessage = new CanbusMessage();
            if(setting->type->id == CanbusMessageType::INT.id) {
                settingMessage->packValue(settingInformation->value->intValue);
            } else if (setting->type->id == CanbusMessageType::FLOAT.id) {
                settingMessage->packValue(settingInformation->value->floatValue);
            } else if (setting->type->id == CanbusMessageType::BOOL.id) {
                settingMessage->packValue(settingInformation->value->boolValue);
            }
            
            node->sendCanbusMessage(settingMessage);
            delete settingMessage;
        }
    }
};

bool CarduinoNodeCanGetSettings::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
