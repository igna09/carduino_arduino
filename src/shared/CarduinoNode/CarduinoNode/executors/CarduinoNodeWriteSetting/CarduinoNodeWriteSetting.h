#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/SettingMessage/SettingMessage.h"
#include "shared/enums/Category.h"
#include "shared/executors/Executor.h"

class CarduinoNodeWriteSetting : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeWriteSetting();

        void execute(CarduinoNode *node, CanbusMessage *message);
};
