#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"

class CarduinoNodeEvent : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeEvent();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
