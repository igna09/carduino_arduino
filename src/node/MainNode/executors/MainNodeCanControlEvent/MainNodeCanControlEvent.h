#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"
#include "MainNode.h"
#include "EventCategory.h"

class MainNodeCanControlEvent : public CarduinoNodeExecutorInterface {
public:
    MainNodeCanControlEvent();
    void execute(CarduinoNode *node, Message *message);
};
