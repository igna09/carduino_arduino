#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"

class CarduinoLogReceivedMessage : public CarduinoNodeExecutorInterface {
    const char* loggerType;
    public:
        CarduinoLogReceivedMessage(const char* loggerType);
        void execute(CarduinoNode *node, Message *message);
        bool canExecute(CarduinoNode *node, Message *message) override;
};
