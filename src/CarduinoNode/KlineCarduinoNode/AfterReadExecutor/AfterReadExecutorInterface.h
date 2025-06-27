#pragma once

#include <Arduino.h>
#include "../../CarduinoNode/CarduinoNode.h"

class AfterReadExecutorInterface {
    public:
        virtual void execute(CarduinoNode*) = 0;
};