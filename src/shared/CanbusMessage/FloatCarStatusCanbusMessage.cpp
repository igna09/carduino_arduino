#include "FloatCarStatusCanbusMessage.h"

FloatCarstatusCanbusMessage::FloatCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CanbusMessage(id, payload, payloadLength) {
    this->carstatus = static_cast<Carstatus>(payload[0]);

    uint8_t sub[this->payloadLength - 1];
    int m;
    pickSubarray(this->payload, sub, m, 1, this->payloadLength);
    this->value = convertByteArrayToFloat(sub);
};
FloatCarstatusCanbusMessage::~FloatCarstatusCanbusMessage() {};

String FloatCarstatusCanbusMessage::getPayloadString() {
    String s = "";
    s += CARSTATUS_STRING[carstatus];
    s += "-";
    s += value;
    return s;
};
