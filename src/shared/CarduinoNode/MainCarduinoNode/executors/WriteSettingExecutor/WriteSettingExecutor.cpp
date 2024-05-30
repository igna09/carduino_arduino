#include "WriteSettingExecutor.h"

WriteSettingExecutor::WriteSettingExecutor() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void WriteSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeWriteSetting WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
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
