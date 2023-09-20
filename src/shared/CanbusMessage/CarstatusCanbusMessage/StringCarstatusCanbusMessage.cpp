#include "StringCarstatusCanbusMessage.h"

StringCarstatusCanbusMessage::StringCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<String>(id, payload, payloadLength, convertByteArrayToString) {};

String StringCarstatusCanbusMessage::toSerialString() {
    String s = "CANBUS;";
    s = s + category->name;
    s = s + ";";
    s = s + carstatus->name;
    s = s + "-";
    s = s + value;
    s = s + ";;";
    return s;
};
