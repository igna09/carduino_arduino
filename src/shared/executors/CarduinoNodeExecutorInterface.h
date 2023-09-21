#pragma once

#include <Arduino.h>
#include "../CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "../CanbusMessage/CanbusMessage.h"

class CarduinoNodeExecutorInterface {
    public:
        virtual void execute(CarduinoNode *node, CanbusMessage message) = 0;
};
