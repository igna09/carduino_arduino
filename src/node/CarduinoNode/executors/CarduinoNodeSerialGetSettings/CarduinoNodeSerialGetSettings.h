#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "EventMulti.h"
#include "MessageType.h"
#include "EventRegistry.h"

class CarduinoNodeSerialGetSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeSerialGetSettings();
        void execute(CarduinoNode *node, Message *message);
};
