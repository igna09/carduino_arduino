#pragma once

#include <Arduino.h>

#include "shared/enums/EventEnum/EventEnum.h"
#include "shared/CanbusMessage/CanbusMessage.h"

class CarduinoNode; //forward declaration, needed to avoid circular dependency
class CarduinoNodeExecutorInterface {
    public:
        const EventEnum* eventEnum;
        bool filterEvent;
        
        CarduinoNodeExecutorInterface();
        CarduinoNodeExecutorInterface(const EventEnum* eventEnum);
        virtual void execute(CarduinoNode *node, CanbusMessage *message) = 0;
        virtual bool canExecute(CarduinoNode *node, CanbusMessage *message) = 0;
};
