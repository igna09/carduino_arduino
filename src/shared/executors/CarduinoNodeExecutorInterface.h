#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"
#include "../enums/Category.h"

class CarduinoNode; //forward declaration, needed to avoid circular dependency
class CarduinoNodeExecutorInterface {
    public:
        const Category *categoryFilter;
        
        CarduinoNodeExecutorInterface(const Category *categoryFilter);
        virtual void execute(CarduinoNode *node, CanbusMessage *message) = 0;
};
