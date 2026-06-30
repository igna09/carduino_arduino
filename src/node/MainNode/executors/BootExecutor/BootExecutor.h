#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "MainNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"

class BootExecutor: public CarduinoNodeExecutorInterface {
public:
    BootExecutor();

    void execute(CarduinoNode *node, Message *message);
};