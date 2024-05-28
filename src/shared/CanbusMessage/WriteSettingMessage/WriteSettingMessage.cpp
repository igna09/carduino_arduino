#include "WriteSettingMessage.h"

/**
 * TODO: improve SettingMessage constructor, ((Setting*) Setting::getValueById(canbusMessage.messageId)) gives nullptr if setting is not existing (this error is valid for every message constructor)
*/
WriteSettingMessage::WriteSettingMessage(CanbusMessage canbusMessage) : TypedCanbusMessage(((Setting*) Setting::getValueById(canbusMessage.messageId))->type, canbusMessage.id, canbusMessage.payload, canbusMessage.payloadLength) {
    this->setting = (Setting*) Setting::getValueById(canbusMessage.messageId);
};

WriteSettingMessage::WriteSettingMessage(const Setting *setting, int value) : TypedCanbusMessage(CarduinoNode::generateId(Category::WRITE_SETTING, *setting), value) {
    this->setting = setting;
};

WriteSettingMessage::WriteSettingMessage(const Setting *setting, float value) : TypedCanbusMessage(CarduinoNode::generateId(Category::WRITE_SETTING, *setting), value) {
    this->setting = setting;
};

WriteSettingMessage::WriteSettingMessage(const Setting *setting, bool value) : TypedCanbusMessage(CarduinoNode::generateId(Category::WRITE_SETTING, *setting), value) {
    this->setting = setting;
};

String WriteSettingMessage::toSerialHumanString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->setting->name;
    s += ";";
    s += this->getValueToString();
    s += ";";
    return s;
};
