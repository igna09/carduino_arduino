#include "CarduinoNodeSerialGetSettings.h"

CarduinoNodeSerialGetSettings::CarduinoNodeSerialGetSettings() : CarduinoNodeExecutorInterface(EV_GET_SETTINGS) {};

void CarduinoNodeSerialGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    // all messages received over serial are sent to canbus from CarduinoNodeSerialEvent
    // node->sendCanbusMessage(message);

    auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(EventRegistry::createById(EV_READ_SETTING));
    std::get<0>(ev->values) = Setting::OTA_MODE.id;
    std::get<1>(ev->values) = node->getSettingValue(&Setting::OTA_MODE)->value->boolValue;

    CanbusMessage *otaModeSettingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);

    node->sendSerialMessage(otaModeSettingMessage);
    delete otaModeSettingMessage;
    
    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeCanGetSettings WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
    std::map<uint8_t, SettingInformation*>::iterator it;
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        if(it->first != Setting::OTA_MODE.id) { // these are managed from main node
            SettingInformation *settingInformation = it->second;
            Setting *setting = (Setting*) Setting::getValueById(it->first);

            CanbusMessage *settingMessage;
            if(setting->type->id == CanbusMessageType::INT.id) {
                auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(EventRegistry::createById(setting->id));
                std::get<0>(ev->values) = it->first;
                std::get<1>(ev->values) = settingInformation->value->intValue;
                settingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);
            } else if (setting->type->id == CanbusMessageType::FLOAT.id) {
                auto* ev = static_cast<EventMulti<uint8_t, float>*>(EventRegistry::createById(setting->id));
                std::get<0>(ev->values) = it->first;
                std::get<1>(ev->values) = settingInformation->value->floatValue;
                settingMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, ev);
            } else if (setting->type->id == CanbusMessageType::BOOL.id) {
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

bool CarduinoNodeSerialGetSettings::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
