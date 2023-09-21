#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"

class CarstatusExecutor : public CarduinoNodeExecutorInterface {
    public:
        void execute(CarduinoNode *node, CanbusMessage message);
};
