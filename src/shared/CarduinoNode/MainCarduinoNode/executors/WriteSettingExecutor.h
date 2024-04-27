#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/SettingMessage/SettingMessage.h"
#include "../../../enums/Category.h"

class WriteSettingExecutor : public CarduinoNodeExecutorInterface {
    public:
        WriteSettingExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
