#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"

class CarduinoNodeSerialEvent : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeSerialEvent();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
