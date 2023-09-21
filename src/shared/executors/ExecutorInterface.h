#pragma once

#include <Arduino.h>

class CanbusMessage; //forward declaration, needed to avoid circular dependency
class ExecutorInterface {
    public:
        virtual void execute(CanbusMessage message) = 0;
};
