#include "HeartbeatMessage.h"

HeartbeatMessage::HeartbeatMessage(uint8_t senderId) : TypedCanbusMessage(CarduinoNode::generateId(Category::HEARTBEAT, senderId), true) {
    this->senderId = senderId;
};

HeartbeatMessage::HeartbeatMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(&CanbusMessageType::BOOL, canbusMessage) {
    this->senderId = this->messageId; // stupid but more clear
};

String HeartbeatMessage::toSerialString() {
    String s = "";
    s += "HEARTBEAT";
    s += ";";
    s += this->senderId;
    s += ";";
    s += this->getValueToString();
    s += ";";
    return s;
};
