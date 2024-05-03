#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/DoorCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/WriteSettingMessage/WriteSettingMessage.h"
#include "shared/enums/Category.h"

class DoorNodeWriteSetting : public CarduinoNodeExecutorInterface {
    public:
        DoorNodeWriteSetting();

        void execute(CarduinoNode *node, CanbusMessage *message);
};
