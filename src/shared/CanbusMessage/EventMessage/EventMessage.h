#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/enums/Carstatus.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/enums/Event.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class EventMessage : public TypedCanbusMessage {
    public:
        const Event *event;

        EventMessage(const Event *event, int senderId);

        String toSerialString();
};
