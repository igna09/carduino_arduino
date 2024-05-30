#pragma once

#include <Arduino.h>
#include "../CanbusMessage/CanbusMessage.h"
#include "CarduinoNodeExecutorInterface.h"
// #include "../CarduinoNode/CarduinoNode/CarduinoNode.h"

/**
 * TODO: change so that is not necessary size anymore
*/
#define EXECUTORS_SIZE 16

class CarduinoNode; //forward declaration to avoid circular dependency
class Executors {
    public:
        CarduinoNodeExecutorInterface* executors[EXECUTORS_SIZE];
        uint8_t size;

        void addExecutor(CarduinoNodeExecutorInterface* e);
        void execute(CarduinoNode *node, CanbusMessage *message);
};
