#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Carstatus.h"
#include "../TypedCanbusMessage/TypedCanbusMessage.h"

class CarstatusMessage : public TypedCanbusMessage {
    public:
        const Carstatus *carstatus;

        CarstatusMessage(const CanbusMessageType*,  CanbusMessage*);
        CarstatusMessage(const Carstatus *carstatus, int value);
        CarstatusMessage(const Carstatus *carstatus, float value);
        CarstatusMessage(const Carstatus *carstatus, bool value);

        String toSerialString() override;
};
