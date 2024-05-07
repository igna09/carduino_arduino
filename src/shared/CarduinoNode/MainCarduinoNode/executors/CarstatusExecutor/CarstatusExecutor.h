#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "shared/enums/Category.h"
#include "shared/enums/Setting.h"

class CarstatusExecutor : public CarduinoNodeExecutorInterface {
    public:
        CarstatusExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
