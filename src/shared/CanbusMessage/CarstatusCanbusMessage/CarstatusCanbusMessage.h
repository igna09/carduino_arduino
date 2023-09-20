#pragma once

#include "../CanbusMessage.h"
#include "../../enums/Carstatus.h"
#include "../../enums/Category.h"
#include "../../utils.h"
#include <Arduino.h>

/**
 * message structure:
 * id -> 11 bit = 3 bit for category (CAR_STATUS, GET_SETTING, ecc.) + 1 byte for message id
 * payload -> 8 byte = 1 byte for Carstatus key + 7 byte for value
*/

template<class T>
class CarstatusCanbusMessage : public CanbusMessage {
    public:
        const Enum *carstatus;
        const Enum *category;
        T *value;
        std::function<T*(uint8_t[], int)> convertByteArrayToTypeFunction;

        CarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength, std::function<T*(uint8_t[], int)> convertByteArrayToTypeFunction);
        CarstatusCanbusMessage();

        virtual String* toSerialString();
};
