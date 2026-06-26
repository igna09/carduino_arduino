#include "CarduinoNodeCanGetSettings.h"

CarduinoNodeCanGetSettings::CarduinoNodeCanGetSettings() : CarduinoNodeExecutorInterface(EV_GET_SETTINGS) {};

void CarduinoNodeCanGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    std::map<uint8_t, SettingInformation*>::iterator it;
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        if(it->first != Setting::OTA_MODE.id) { // these are managed from main node
            SettingInformation *settingInformation = it->second;
            Setting *setting = (Setting*) Setting::getValueById(it->first);

            CanbusMessage *settingMessage;
            if(setting->type->id == MessageType::INT.id) {
                auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(EventRegistry::createById(setting->id));
                std::get<0>(ev->values) = it->first;
                std::get<1>(ev->values) = settingInformation->value->intValue;
                settingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);
            } else if (setting->type->id == MessageType::FLOAT.id) {
                auto* ev = static_cast<EventMulti<uint8_t, float>*>(EventRegistry::createById(setting->id));
                std::get<0>(ev->values) = it->first;
                std::get<1>(ev->values) = settingInformation->value->floatValue;
                settingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);
            } else if (setting->type->id == MessageType::BOOL.id) {
                auto* ev = static_cast<EventMulti<uint8_t, bool>*>(EventRegistry::createById(setting->id));
                std::get<0>(ev->values) = it->first;
                std::get<1>(ev->values) = settingInformation->value->boolValue;
                settingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);
            }
            
            node->sendSerialMessage(settingMessage);
            delete settingMessage;
        }
    }
};
