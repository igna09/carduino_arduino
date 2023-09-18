#include "CarstatusCanbusMessageFactory.h"

template<class T>
CarstatusCanbusMessage<T>* CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(CarstatusCanbusMessageType type, unsigned long id, uint8_t *payload, uint8_t payloadLength) {
    CarstatusCanbusMessage<T> *carstatusCanbusMessage;

    if(type == CarstatusCanbusMessageType::FLOAT) {
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToFloat);
    } else if(type == CarstatusCanbusMessageType::FLOAT) {
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToInt);
    } else if(type == CarstatusCanbusMessageType::FLOAT) {
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToString);
    }

    return carstatusCanbusMessage;
}