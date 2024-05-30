#include "CarduinoNodeWriteSetting.h"

CarduinoNodeWriteSetting::CarduinoNodeWriteSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void CarduinoNodeWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    WriteSettingMessage *settingMessage = new WriteSettingMessage(*message);

    if(settingMessage->setting->type->id == CanbusMessageType::INT.id) {
        node->putSettingValue(*settingMessage->setting, settingMessage->getIntValue());
    } else if (settingMessage->setting->type->id == CanbusMessageType::FLOAT.id) {
        node->putSettingValue(*settingMessage->setting, settingMessage->getFloatValue());
    } else if (settingMessage->setting->type->id == CanbusMessageType::BOOL.id) {
        node->putSettingValue(*settingMessage->setting, settingMessage->getBoolValue());
    }

    delete settingMessage;
};
