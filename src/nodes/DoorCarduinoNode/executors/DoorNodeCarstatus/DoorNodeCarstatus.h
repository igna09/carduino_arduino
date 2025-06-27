#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "nodes/DoorCarduinoNode/DoorCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Carstatus.h"
#include "shared/CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "shared/enums/Category.h"

class DoorNodeCarstatus : public CarduinoNodeExecutorInterface {
    public:
        DoorNodeCarstatus();

        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
