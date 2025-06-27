#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
// #include "../../../CanbusMessage/SettingMessage/SettingMessage.h"
#include "CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"

class AllMessageExecutor : public CarduinoNodeExecutorInterface {
    public:
        AllMessageExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
