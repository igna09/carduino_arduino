#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "MainNode.h"

class SpeedLimitSet : public CarduinoNodeExecutorInterface {
public:
    SpeedLimitSet();
    void execute(CarduinoNode *node, Message *message);
};
