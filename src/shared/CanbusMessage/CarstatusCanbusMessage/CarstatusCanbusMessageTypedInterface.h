#pragma once

#include "Arduino.h"
#include "../CanbusMessage.h"

class CarstatusCanbusMessageTypedInterface : public CanbusMessage {
    public:
        virtual String toSerialString() = 0;
        virtual ~CarstatusCanbusMessageTypedInterface() = default;
};
