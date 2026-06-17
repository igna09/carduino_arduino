#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/EventEnum/EventEnum.h"
#include "shared/enums/Setting.h"

class CarduinoNodeCanGetSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeCanGetSettings();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
