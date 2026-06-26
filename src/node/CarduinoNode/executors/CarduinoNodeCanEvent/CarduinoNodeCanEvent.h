#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"

class CarduinoNodeCanEvent : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeCanEvent();
        void execute(CarduinoNode *node, Message *message);
        bool canExecute(CarduinoNode *node, Message *message) override;
};
