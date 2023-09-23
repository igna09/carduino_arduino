#include "CarstatusMessage.h"

CarstatusMessage::CarstatusMessage(const CanbusMessageType *type, CanbusMessage *canbusMessage) : TypedCanbusMessage(type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->carstatus = (Carstatus*) Carstatus::getValueById(canbusMessage->messageId);
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
