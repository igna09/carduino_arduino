#pragma once

#include <Arduino.h>
#include "CarstatusCanbusMessage.h"
#include "IntCarstatusCanbusMessage.h"
#include "FloatCarstatusCanbusMessage.h"
#include "StringCarstatusCanbusMessage.h"
#include "CarstatusCanbusMessageTypedInterface.h"
#include "../../enums/CarstatusCanbusMessageType.h"

class CarstatusCanbusMessageFactory {
    public:
        static CarstatusCanbusMessageTypedInterface* getCarstatusCanbusMessage(CarstatusCanbusMessageType type, unsigned long id, uint8_t *payload, uint8_t payloadLength);
};
