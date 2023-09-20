#include "IntCarstatusCanbusMessage.h"

IntCarstatusCanbusMessage::IntCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength): CarstatusCanbusMessageTypedInterface(), CarstatusCanbusMessage<int>(id, payload, payloadLength, convertByteArrayToInt) {};

String* IntCarstatusCanbusMessage::toSerialString() {
    String *s = new String("CANBUS;");
    *s = *s + category->name;
    *s = *s + ";";
    *s = *s + carstatus->name;
    *s = *s + "-";
    *s = *s + *value;
    *s = *s + ";;";
    return s;
};
