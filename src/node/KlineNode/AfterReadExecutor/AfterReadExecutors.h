#pragma once

#include <vector>
#include <memory>

#include "AfterReadExecutorInterface.h"
#include "CarduinoNode.h"

#define AFTER_READ_EXECUTORS_SIZE 8

class AfterReadExecutors {
    public:
        std::vector<std::shared_ptr<AfterReadExecutorInterface>> executors;

        AfterReadExecutors();

        void addExecutor(std::shared_ptr<AfterReadExecutorInterface> executor);
        void execute(CarduinoNode *node);
};
