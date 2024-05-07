#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/DoorCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Carstatus.h"
#include "shared/CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "shared/enums/Category.h"

class DoorNodeCarstatus : public CarduinoNodeExecutorInterface {
    public:
        DoorNodeCarstatus();

        void execute(CarduinoNode *node, CanbusMessage *message);
};
