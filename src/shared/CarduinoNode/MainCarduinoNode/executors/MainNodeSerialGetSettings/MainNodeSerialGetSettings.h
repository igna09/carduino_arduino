#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/ReadSettingMessage/ReadSettingMessage.h"
#include "shared/enums/Category.h"

class MainNodeSerialGetSettings : public CarduinoNodeExecutorInterface {
    public:
        MainNodeSerialGetSettings();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
