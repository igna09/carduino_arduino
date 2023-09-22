#include "CarstatusMessage.h"

CarstatusMessage::CarstatusMessage(const CanbusMessageType *type, CanbusMessage *canbusMessage) : TypedCanbusMessage(type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    Serial.println("carstatus 0");
    this->carstatus = (Carstatus*) Carstatus::getValueById(canbusMessage->messageId);
    Serial.print("carstatus ");
    Serial.print(canbusMessage->messageId);
    Serial.print(" ");
    Serial.println(this->carstatus->name);
    const CanbusMessageType *t = this->carstatus->type;
    Serial.print("type ");
    Serial.println(t->name);
};

String CarstatusMessage::toSerialString() {
    Serial.println("CarstatusMessage toSerialString");
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->carstatus->name;
    s += "-";
    s += this->getValueToString();
    s += ";";
    return s;
};
