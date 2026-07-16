#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "EventMulti.h"
#include "MessageType.h"
#include "SendSettingsUpdate.h"

class CarduinoNodeCanGetSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeCanGetSettings();
        void execute(CarduinoNode *node, Message *message);
};
