#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"
#include "MainNode.h"
#include "EventCategory.h"

class MainNodeCanEvent : public CarduinoNodeExecutorInterface {
public:
    MainNodeCanEvent();
    void execute(CarduinoNode *node, Message *message);
    bool canExecute(CarduinoNode *node, Message *message);
};
