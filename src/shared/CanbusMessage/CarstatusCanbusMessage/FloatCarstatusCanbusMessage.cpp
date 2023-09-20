#include "FloatCarstatusCanbusMessage.h"

FloatCarstatusCanbusMessage::FloatCarstatusCanbusMessage() : CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<float>() {};
FloatCarstatusCanbusMessage::FloatCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<float>(id, payload, payloadLength, convertByteArrayToFloat) {};

String FloatCarstatusCanbusMessage::toSerialString() {
    String s = "CANBUS;";
    s = s + category->name;
    s = s + ";";
    s = s + carstatus->name;
    s = s + "-";
    s = s + String(value);
    s = s + ";;";
    return s;
};
