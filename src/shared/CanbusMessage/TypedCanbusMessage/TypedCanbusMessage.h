#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Carstatus.h"
#include "../../enums/Category.h"
#include "../../utils.h"
#include "../../enums/CanbusMessageType.h"
#include "ValueTypedBase.h"

/**
 * message structure:
 * id -> 11 bit = 3 bit for category (CAR_STATUS, GET_SETTING, ecc.) + 1 byte for message id
 * payload -> 8 byte = 1 byte for Carstatus key + 7 byte for value
 */

class TypedCanbusMessage : public CanbusMessage, public ValueTypedBase
{
public:
    const Category *category;

    TypedCanbusMessage(const CanbusMessageType *type, unsigned long id, uint8_t *payload, uint8_t payloadLength);
    TypedCanbusMessage(const CanbusMessageType *type, CanbusMessage *message);
    TypedCanbusMessage(unsigned long id, int value);
    TypedCanbusMessage(unsigned long id, bool value);
    TypedCanbusMessage(unsigned long id, float value);
    TypedCanbusMessage();
    virtual ~TypedCanbusMessage() = default;
};
