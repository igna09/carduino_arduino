#include "SettingMessage.h"

/**
 * TODO: improve SettingMessage constructor, ((Setting*) Setting::getValueById(canbusMessage.messageId)) gives nullptr if setting is not existing (this error is valid for every message constructor)
*/
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
