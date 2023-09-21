#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"
#include "CarduinoNodeExecutorInterface.h"

class Executors : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeExecutorInterface* executors[4];
        uint8_t size;

        void addExecutor(CarduinoNodeExecutorInterface* e);
        void execute(CarduinoNode *node, CanbusMessage message);
};
