#pragma once

#include <Arduino.h>

#include "shared/CanbusMessage/CanbusMessage.h"

class CarduinoNode; //forward declaration, needed to avoid circular dependency
class CarduinoNodeExecutorInterface {
    public:
        uint8_t eventId;
        bool filterEvent;
        
        CarduinoNodeExecutorInterface();
        CarduinoNodeExecutorInterface(uint8_t eventId);
        virtual void execute(CarduinoNode *node, CanbusMessage *message) = 0;
        virtual bool canExecute(CarduinoNode *node, CanbusMessage *message) = 0;
};
