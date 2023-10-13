#include "SettingMessage.h"

SettingMessage::SettingMessage(CanbusMessage canbusMessage) : TypedCanbusMessage(((Setting*) Setting::getValueById(canbusMessage.messageId))->type, canbusMessage.id, canbusMessage.payload, canbusMessage.payloadLength) {
    this->setting = (Setting*) Setting::getValueById(canbusMessage.messageId);
};

String SettingMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->setting->name;
    s += ";";
    s += this->getValueToString();
    s += ";";
    return s;
};
