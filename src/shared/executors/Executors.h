#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"
#include "CarduinoNodeExecutorInterface.h"
#include "../CarduinoNode/CarduinoNode/CarduinoNode.h"

#define EXECUTORS_SIZE 8

class Executors {
    public:
        CarduinoNodeExecutorInterface* executors[EXECUTORS_SIZE];
        uint8_t size;

        void addExecutor(CarduinoNodeExecutorInterface* e);
        void execute(CarduinoNode *node, CanbusMessage *message);
};
