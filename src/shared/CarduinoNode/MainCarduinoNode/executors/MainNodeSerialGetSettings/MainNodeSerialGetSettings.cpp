#include "MainNodeSerialGetSettings.h"

MainNodeSerialGetSettings::MainNodeSerialGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void MainNodeSerialGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    ReadSettingMessage *otaModeSettingMessage = new ReadSettingMessage(&Setting::OTA_MODE, node->getSettingValue(Setting::OTA_MODE)->valueType->boolValue);
    ((MainCarduinoNode*)node)->sendSerialMessage(otaModeSettingMessage);
    delete otaModeSettingMessage;


    ReadSettingMessage *restartSettingMessage = new ReadSettingMessage(&Setting::RESTART, node->getSettingValue(Setting::RESTART)->valueType->boolValue);
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

            ReadSettingMessage *readSettingMessage = nullptr;
            if(setting->type->id == CanbusMessageType::INT.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->intValue);
            } else if (setting->type->id == CanbusMessageType::FLOAT.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->floatValue);
            } else if (setting->type->id == CanbusMessageType::BOOL.id) {
                readSettingMessage = new ReadSettingMessage(setting, settingInformation->valueType->boolValue);
            }
            
            mainCarduinoNode->sendSerialMessage(readSettingMessage);
            delete readSettingMessage;
        }
    }
};
