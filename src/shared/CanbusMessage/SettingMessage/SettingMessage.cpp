#include "SettingMessage.h"

SettingMessage::SettingMessage(CanbusMessage canbusMessage) : TypedCanbusMessage((CanbusMessageType*)CanbusMessageType::getValueById(canbusMessage.messageId), id, payload, payloadLength) {
    this->setting = (Setting*) Setting::getValueById(canbusMessage.messageId);
};

String SettingMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->setting->name;
    s += ";";
    s += this->getValueToString();
    return s;
};
