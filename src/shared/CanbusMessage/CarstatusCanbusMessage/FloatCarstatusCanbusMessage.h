#pragma once

#include "CarstatusCanbusMessage.h"
#include "Arduino.h"
#include "../../utils.h"
#include "CarstatusCanbusMessageTypedInterface.h"

/**
 * message structure:
 * id -> 11 bit = 3 bit for category (CAR_STATUS, GET_SETTING, ecc.) + 1 byte for message id
 * payload -> 8 byte = 1 byte for Carstatus key + 7 byte for value
*/

class FloatCarstatusCanbusMessage : public CarstatusCanbusMessageTypedInterface, public CarstatusCanbusMessage<float> {
    public:
        FloatCarstatusCanbusMessage();
        FloatCarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength);

        String* toSerialString();
};
