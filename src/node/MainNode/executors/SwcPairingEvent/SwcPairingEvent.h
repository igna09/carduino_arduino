#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "MainNode.h"

class SwcPairingEvent : public CarduinoNodeExecutorInterface {
public:
    SwcPairingEvent();
    void execute(CarduinoNode *node, Message *message);
};
