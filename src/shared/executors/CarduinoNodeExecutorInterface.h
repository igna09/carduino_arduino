#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"

class CarduinoNode; //forward declaration, needed to avoid circular dependency
class CarduinoNodeExecutorInterface {
    public:
        virtual void execute(CarduinoNode *node, CanbusMessage message) = 0;
};
