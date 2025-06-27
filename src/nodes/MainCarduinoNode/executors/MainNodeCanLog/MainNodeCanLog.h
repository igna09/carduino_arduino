#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "nodes/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"
#include "shared/CanbusMessage/LogMessage/LogMessage.h"

class MainNodeCanLog : public CarduinoNodeExecutorInterface {
    public:
        MainNodeCanLog();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
