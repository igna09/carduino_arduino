#pragma once

#include <Arduino.h>
#include "CarstatusCanbusMessage.h"
#include "../../enums/CarstatusCanbusMessageType.h"

class CarstatusCanbusMessageFactory {
    public:
        template <class T>
        static CarstatusCanbusMessage<T>* getCarstatusCanbusMessage(CarstatusCanbusMessageType type, unsigned long id, uint8_t *payload, uint8_t payloadLength);
};
