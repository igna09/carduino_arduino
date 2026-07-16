#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "EventMulti.h"
#include "MessageType.h"
#include "EventRegistry.h"
#include "SendSettingsUpdate.h"

class CarduinoNodeSerialSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeSerialSettings();
        void execute(CarduinoNode *node, Message *message);
    private:
        CarduinoNode *node;
        void getSettings();
        void writeSetting(Message *m);
};

