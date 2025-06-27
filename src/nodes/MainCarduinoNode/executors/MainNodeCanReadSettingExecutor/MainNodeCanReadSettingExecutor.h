#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CanbusMessage/SettingMessage/SettingMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "nodes/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"

class MainNodeCanReadSettingExecutor : public CarduinoNodeExecutorInterface {
    public:
        MainNodeCanReadSettingExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
