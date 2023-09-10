#include "CarstatusCanbusMessageFactory.h"

template<class T>
CarstatusCanbusMessage<T>* CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(CarstatusCanbusMessageType type, unsigned long id, uint8_t *payload, uint8_t payloadLength) {
    CarstatusCanbusMessage<T> *carstatusCanbusMessage;
    
    switch (type)
    {
    case CarstatusCanbusMessageType::FLOAT:
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToFloat);
        break;
    
    case CarstatusCanbusMessageType::INT:
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToInt);
        break;
    
    case CarstatusCanbusMessageType::STRING:
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToString);
        break;
    }

    return carstatusCanbusMessage;
}