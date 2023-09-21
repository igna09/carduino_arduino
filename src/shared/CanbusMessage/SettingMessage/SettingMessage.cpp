#include "SettingMessage.h"

SettingMessage::SettingMessage(CanbusMessage canbusMessage) : TypedCanbusMessage((CanbusMessageType*)CanbusMessageType::getValueById(canbusMessage.messageId), id, payload, payloadLength) {
    this->setting = (Setting*) Setting::getValueById(canbusMessage.messageId);
};
