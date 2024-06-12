#pragma once

#include <Arduino.h>
#include "AfterReadExecutorInterface.h"
#include "../../CarduinoNode/CarduinoNode.h"

#define AFTER_READ_EXECUTORS_SIZE 8

class AfterReadExecutors {
    public:
        AfterReadExecutorInterface* executors[AFTER_READ_EXECUTORS_SIZE];
        uint8_t size = 0;

        void addExecutor(AfterReadExecutorInterface* e);
        void execute(CarduinoNode *node);
};
