#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "Executor.h"
#include "EventMulti.h"
#include "MessageType.h"

class CarduinoNodeWriteSetting : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeWriteSetting();

        void execute(CarduinoNode *node, Message *message) override;
};
