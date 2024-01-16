#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "../TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/enums/MediaControl.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class HeartbeatMessage : public TypedCanbusMessage {
    public:
        uint8_t senderId;

        HeartbeatMessage(uint8_t senderId);
        String toSerialString();
};
