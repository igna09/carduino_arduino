#pragma once

#include "CarduinoNode.h"

class AfterReadExecutorInterface {
    public:
        virtual void execute(CarduinoNode*) = 0;
};