#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"
#include "shared/CanbusMessage/LogMessage/LogMessage.h"

class MainNodeCanLog : public CarduinoNodeExecutorInterface {
    public:
        MainNodeCanLog();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
