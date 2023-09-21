#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../../CanbusMessage/CarstatusCanbusMessage/CarstatusCanbusMessageFactory.h"
#include "../../../CanbusMessage/CarstatusCanbusMessage/CarstatusCanbusMessageTypedInterface.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"

class CarstatusExecutor : public CarduinoNodeExecutorInterface {
    public:
        void execute(CarduinoNode *node, CanbusMessage message);
};
