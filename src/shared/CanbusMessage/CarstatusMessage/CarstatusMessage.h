#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Carstatus.h"
#include "../TypedCanbusMessage/TypedCanbusMessage.h"

class CarstatusMessage : public TypedCanbusMessage {
    public:
        const Carstatus *carstatus;

        CarstatusMessage(const CanbusMessageType*,  CanbusMessage*);
        String toSerialString() override;
};
