#include "MainNodeSerialGetSettings.h"

MainNodeSerialGetSettings::MainNodeSerialGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void MainNodeSerialGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    SettingMessage *otaModeSettingMessage = new SettingMessage(&Setting::OTA_MODE, true, node->getSettingValue(&Setting::OTA_MODE)->value->boolValue);
    ((MainCarduinoNode*)node)->sendSerialMessage(otaModeSettingMessage);
    delete otaModeSettingMessage;


    SettingMessage *restartSettingMessage = new SettingMessage(&Setting::RESTART, true, node->getSettingValue(&Setting::RESTART)->value->boolValue);
    ((MainCarduinoNode*)node)->sendSerialMessage(restartSettingMessage);
    delete restartSettingMessage;

    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*) node;
    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeCanGetSettings WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
    std::map<uint8_t, SettingInformation*>::iterator it;
    for (it = node->settings->begin(); it != node->settings->end(); it++) {
        if(it->first != Setting::OTA_MODE.id && it->first != Setting::RESTART.id) { // these are managed from main node
            SettingInformation *settingInformation = it->second;
            Setting *setting = (Setting*) Setting::getValueById(it->first);

            SettingMessage *settingMessage = nullptr;
            if(setting->type->id == CanbusMessageType::INT.id) {
                settingMessage = new SettingMessage(setting, true, settingInformation->value->intValue);
            } else if (setting->type->id == CanbusMessageType::FLOAT.id) {
                settingMessage = new SettingMessage(setting, true, settingInformation->value->floatValue);
            } else if (setting->type->id == CanbusMessageType::BOOL.id) {
                settingMessage = new SettingMessage(setting, true, settingInformation->value->boolValue);
            }
            
            mainCarduinoNode->sendSerialMessage(settingMessage);
            delete settingMessage;
        }
    }
};
