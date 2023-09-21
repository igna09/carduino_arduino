#include "BoolCarstatusCanbusMessage.h"

BoolCarstatusCanbusMessage::BoolCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<bool>(id, payload, payloadLength, convertByteArrayToBool) {};

String BoolCarstatusCanbusMessage::toSerialString() {
    String s = "CANBUS;";
    s = s + category->name;
    s = s + ";";
    s = s + carstatus->name;
    s = s + "-";
    s = s + value ? "true" : "false";
    s = s + ";;";
    return s;
};
