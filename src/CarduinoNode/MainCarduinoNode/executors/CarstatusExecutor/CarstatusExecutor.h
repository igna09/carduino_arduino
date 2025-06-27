#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "shared/enums/Category.h"
#include "shared/enums/Setting.h"

class CarstatusExecutor : public CarduinoNodeExecutorInterface {
    public:
        CarstatusExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
