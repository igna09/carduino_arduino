#include "AfterReadExecutor.h"

void AfterReadExecutors::addExecutor(AfterReadExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void AfterReadExecutors::execute(CarduinoNode *node) {
    if(node->isEnabled) {
        for(uint8_t i = 0; i < this->size; i++) {
            executors[i]->execute(node);
        }
    }
}
