#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
// #include "../../../CanbusMessage/SettingMessage/SettingMessage.h"

class AllMessageExecutor : public CarduinoNodeExecutorInterface {
    public:
        AllMessageExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
