#include "./CanbusMessage.h"

CanbusMessage::CanbusMessage(unsigned long id, unsigned char *payload, uint8_t payloadLength) {
    this->id = id;
    this->payload = payload;
    this->payloadLength = payloadLength;
    this->category = static_cast<Category>(this->id>>8);
};
CanbusMessage::~CanbusMessage() {};

String CanbusMessage::toString() {
    String s = "";
    s += "CANBUS;CAR_STATUS;";
    s += getPayloadString();
    s += ";;";
    return s;
}

String CanbusMessage::getPayloadString() {
    return "";
};