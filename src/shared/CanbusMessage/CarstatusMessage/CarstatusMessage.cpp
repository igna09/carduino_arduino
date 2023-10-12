#include "CarstatusMessage.h"

CarstatusMessage::CarstatusMessage(const CanbusMessageType *type, CanbusMessage *canbusMessage) : TypedCanbusMessage(type, canbusMessage) {
    this->carstatus = (Carstatus*) Carstatus::getValueById(canbusMessage->messageId);
};

CarstatusMessage::CarstatusMessage(const Carstatus *carstatus, int value) : TypedCanbusMessage(((Carstatus*)carstatus)->getMessageId(), value) {
    this->carstatus = carstatus;
};

CarstatusMessage::CarstatusMessage(const Carstatus *carstatus, bool value) : TypedCanbusMessage(((Carstatus*)carstatus)->getMessageId(), value) {
    this->carstatus = carstatus;
};

CarstatusMessage::CarstatusMessage(const Carstatus *carstatus, float value) : TypedCanbusMessage(((Carstatus*)carstatus)->getMessageId(), value) {
    this->carstatus = carstatus;
};

CarstatusMessage::CarstatusMessage() : TypedCanbusMessage() {};

String CarstatusMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->carstatus->name;
    s += ";";
    s += this->getValueToString();
    s += ";";
    return s;
};
