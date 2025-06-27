#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/enums/Carstatus.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/enums/Event.h"
#include "nodes/CarduinoNode/CarduinoNode.h"

class EventMessage : public TypedCanbusMessage {
    public:
        const Event *event;
        int nodeId;

        EventMessage(const Event *event, int nodeId);
        EventMessage(CanbusMessage *canbusMessage);

        String toSerialHumanString() override;
        static EventMessage* createSpecializedCopy(CanbusMessage*);
};
