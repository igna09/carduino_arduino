#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Node.h"

class CarduinoNodeSerialEvent : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeSerialEvent();
        void execute(CarduinoNode *node, Message *message);
};
