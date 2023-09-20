#include "CarstatusCanbusMessageFactory.h"


CarstatusCanbusMessageTypedInterface* CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(CarstatusCanbusMessageType type, unsigned long id, uint8_t *payload, uint8_t payloadLength) {
    if(type == CarstatusCanbusMessageType::FLOAT) {
        return new FloatCarstatusCanbusMessage(id, payload, payloadLength);
    } else if(type == CarstatusCanbusMessageType::INT) {
        return new IntCarstatusCanbusMessage(id, payload, payloadLength);
    } else if(type == CarstatusCanbusMessageType::STRING) {
        return new StringCarstatusCanbusMessage(id, payload, payloadLength);
    }

    return nullptr;
}