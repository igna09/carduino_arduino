#include "ReadSettingMessage.h"

/**
 * TODO: improve SettingMessage constructor, ((Setting*) Setting::getValueById(canbusMessage.messageId)) gives nullptr if setting is not existing (this error is valid for every message constructor)
*/
ReadSettingMessage::ReadSettingMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(((Setting*) Setting::getValueById(canbusMessage->messageId))->type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->setting = (Setting*) Setting::getValueById(canbusMessage->messageId);
};

ReadSettingMessage::ReadSettingMessage(const Setting *setting, int value) : TypedCanbusMessage(CarduinoNode::generateId(Category::READ_SETTING, *setting), value) {
    this->setting = setting;
};

ReadSettingMessage::ReadSettingMessage(const Setting *setting, float value) : TypedCanbusMessage(CarduinoNode::generateId(Category::READ_SETTING, *setting), value) {
    this->setting = setting;
};

ReadSettingMessage::ReadSettingMessage(const Setting *setting, bool value) : TypedCanbusMessage(CarduinoNode::generateId(Category::READ_SETTING, *setting), value) {
    this->setting = setting;
};

String ReadSettingMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->setting->name;
    s += ";";
    s += this->getValueToString();
    s += ";";
    return s;
};
