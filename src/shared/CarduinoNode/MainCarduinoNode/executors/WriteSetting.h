#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/SettingMessage/SettingMessage.h"
#include "../../../enums/Category.h"

class WriteSetting : public CarduinoNodeExecutorInterface {
    public:
        WriteSetting(const Category *categoryFilter);
        void execute(CarduinoNode *node, CanbusMessage message);
};