#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"
#include "shared/CanbusMessage/ReadSettingMessage/ReadSettingMessage.h"

class CarduinoNodeCanGetSettings : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeCanGetSettings();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
