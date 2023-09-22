#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "../../../enums/Category.h"

class CarstatusExecutor : public CarduinoNodeExecutorInterface {
    public:
        CarstatusExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
