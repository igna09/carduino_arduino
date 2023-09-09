#include "CarstatusCanbusMessageFactory.h"

template<class T>
CarstatusCanbusMessage<T>* CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(String type, unsigned long id, uint8_t *payload, uint8_t payloadLength) {
    CarstatusCanbusMessage<T> *carstatusCanbusMessage;
    
    switch (type)
    {
    case "float":
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToFloat);
        break;
    
    case "int":
        carstatusCanbusMessage = new CarstatusCanbusMessage<T>(id, payload, payloadLength, convertByteArrayToInt);
        break;
    }

    return carstatusCanbusMessage;
}