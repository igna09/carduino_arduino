#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"
#include "../enums/Category.h"
// #include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class CarduinoNode; //forward declaration, needed to avoid circular dependency
class CarduinoNodeExecutorInterface {
    public:
        const Category *categoryFilter;
        uint8_t messageId;
        bool filterMessage;
        bool needNodeEnabled;
        
        CarduinoNodeExecutorInterface(const Category *categoryFilter);
        CarduinoNodeExecutorInterface(const Category *categoryFilter, uint8_t messageId);
        virtual void execute(CarduinoNode *node, CanbusMessage *message) = 0;
        virtual bool canExecute(CarduinoNode *node, CanbusMessage *message) = 0;
};
