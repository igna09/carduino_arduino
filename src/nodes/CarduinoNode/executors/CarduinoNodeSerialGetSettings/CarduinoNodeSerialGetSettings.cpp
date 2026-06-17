#include "CarduinoNodeSerialGetSettings.h"

CarduinoNodeSerialGetSettings::CarduinoNodeSerialGetSettings() : CarduinoNodeExecutorInterface(&EventEnum::GET_SETTINGS) {};

void CarduinoNodeSerialGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    CanbusMessage *otaModeSettingMessage = new CanbusMessage();
    otaModeSettingMessage->eventId = EventEnum::READ_SETTING.id;
    otaModeSettingMessage->packValue(Setting::OTA_MODE.id);
    otaModeSettingMessage->packValue(DataTypeEnum::BOOL.id);
    otaModeSettingMessage->packValue(node->getSettingValue(&Setting::OTA_MODE)->value->boolValue);
    node->sendSerialMessage(otaModeSettingMessage);
    delete otaModeSettingMessage;


    // SettingMessage *restartSettingMessage = new SettingMessage(&Setting::RESTART, true, node->getSettingValue(&Setting::RESTART)->value->boolValue);
    // node->sendSerialMessage(restartSettingMessage);
    // delete restartSettingMessage;
    
    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeCanGetSettings WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
    std::map<uint8_t, SettingInformation*>::iterator it;
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        if(it->first != Setting::OTA_MODE.id) { // these are managed from main node
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
            
            node->sendSerialMessage(settingMessage);
            delete settingMessage;
        }
    }
};

bool CarduinoNodeSerialGetSettings::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
