#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/SettingMessage/SettingMessage.h"

class WriteSetting : public CarduinoNodeExecutorInterface {
    public:
        void execute(CarduinoNode *node, CanbusMessage message);
};
