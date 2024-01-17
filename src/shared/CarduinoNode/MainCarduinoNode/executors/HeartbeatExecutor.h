#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/HeartbeatMessage/HeartbeatMessage.h"
#include "../../../enums/Category.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include <map>

class HeartbeatExecutor : public CarduinoNodeExecutorInterface {
    public:
        HeartbeatExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
