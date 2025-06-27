#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/enums/Category.h"
#include "CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include <map>

class HeartbeatExecutor : public CarduinoNodeExecutorInterface {
    public:
        HeartbeatExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
