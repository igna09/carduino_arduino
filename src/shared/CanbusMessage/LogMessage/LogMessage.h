#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/enums/Carstatus.h"
#include "shared/enums/Event.h"
#include "CarduinoNode/CarduinoNode/CarduinoNode.h"

class LogMessage : public CanbusMessage, public ValueTypedBase {
    public:
        uint8_t logId;
        uint8_t nodeId;

        LogMessage(uint8_t nodeId, uint8_t logId, int value);
        LogMessage(uint8_t nodeId, uint8_t logId, bool value);
        LogMessage(uint8_t nodeId, uint8_t logId, float value);
        LogMessage(CanbusMessage *canbusMessage);

        String toSerialHumanString() override;
        String toSerialString() override;
        static LogMessage* createSpecializedCopy(CanbusMessage*);
};
