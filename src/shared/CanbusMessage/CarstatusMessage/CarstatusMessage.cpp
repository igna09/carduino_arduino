#include "CarstatusMessage.h"

CarstatusMessage::CarstatusMessage(CanbusMessage canbusMessage) : TypedCanbusMessage((CanbusMessageType*)CanbusMessageType::getValueById(canbusMessage.messageId), id, payload, payloadLength) {
    this->carstatus = (Carstatus*) Carstatus::getValueById(canbusMessage.messageId);
};

String CarstatusMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->carstatus->name;
    s += "-";
    s += this->getValueToString();
    s += ";";
    return s;
};
