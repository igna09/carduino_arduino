#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "EventMulti.h"
#include "MessageType.h"
#include "EventRegistry.h"
#include "SendSettingsUpdate.h"

class CarduinoNodeCanSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeCanSettings();
        void execute(CarduinoNode *node, Message *message);
    private:
        CarduinoNode *node;
        void readSetting(Message *message);
        void getSettings();
        void writeSetting(Message *message);
};

