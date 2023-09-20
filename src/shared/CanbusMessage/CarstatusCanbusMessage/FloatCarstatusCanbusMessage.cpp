#include "FloatCarstatusCanbusMessage.h"

FloatCarstatusCanbusMessage::FloatCarstatusCanbusMessage() : CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<float>() {};
FloatCarstatusCanbusMessage::FloatCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<float>(id, payload, payloadLength, convertByteArrayToFloat) {};

String* FloatCarstatusCanbusMessage::toSerialString() {
    String *s = new String("CANBUS;");
    *s = *s + category->name;
    *s = *s + ";";
    *s = *s + carstatus->name;
    *s = *s + "-";
    *s = *s + *value;
    *s = *s + ";;";
    return s;
};
