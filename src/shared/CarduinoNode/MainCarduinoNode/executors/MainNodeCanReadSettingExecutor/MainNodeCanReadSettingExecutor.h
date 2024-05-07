#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"

class MainNodeCanReadSettingExecutor : public CarduinoNodeExecutorInterface {
    public:
        MainNodeCanReadSettingExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
