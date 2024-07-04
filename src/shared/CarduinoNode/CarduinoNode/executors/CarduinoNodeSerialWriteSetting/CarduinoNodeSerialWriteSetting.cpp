#include "CarduinoNodeSerialWriteSetting.h"

CarduinoNodeSerialWriteSetting::CarduinoNodeSerialWriteSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void CarduinoNodeSerialWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeWriteSetting WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
    SettingMessage *settingMessage = new SettingMessage(message);

    if(settingMessage->setting->type->id == CanbusMessageType::INT.id) {
        node->putSettingValue(settingMessage->setting, settingMessage->getIntValue());
    } else if (settingMessage->setting->type->id == CanbusMessageType::FLOAT.id) {
        node->putSettingValue(settingMessage->setting, settingMessage->getFloatValue());
    } else if (settingMessage->setting->type->id == CanbusMessageType::BOOL.id) {
        node->putSettingValue(settingMessage->setting, settingMessage->getBoolValue());
    }

    delete settingMessage;
};

bool CarduinoNodeSerialWriteSetting::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
