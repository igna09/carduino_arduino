#pragma once

#include <Arduino.h>
#include "../enums/Category.h"
#include "../enums/Carstatus.h"

/**
 * message structure:
 * id -> 11 bit = 3 bit for category (CAR_STATUS, GET_SETTING, ecc.) + 1 byte for message id
 * payload -> 8 byte
*/

class CanbusMessage
{
public:
    unsigned long id;
    unsigned char *payload;
    uint8_t payloadLength;
    const Category *category;

    CanbusMessage(unsigned long id, unsigned char *payload, uint8_t payloadLength);
    ~CanbusMessage();
    String toString();
    String getPayloadString();
};
