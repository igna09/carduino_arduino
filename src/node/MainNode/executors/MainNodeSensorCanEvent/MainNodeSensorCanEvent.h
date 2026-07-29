#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"
#include "MainNode.h"
#include "EventCategory.h"

class MainNodeSensorCanEvent : public CarduinoNodeExecutorInterface {
public:
    MainNodeSensorCanEvent();
    void execute(CarduinoNode *node, Message *message);
};
