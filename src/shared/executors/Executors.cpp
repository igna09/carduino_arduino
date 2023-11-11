#include "Executors.h"

void Executors::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void Executors::execute(CarduinoNode *node, CanbusMessage *message) {
    for(uint8_t i = 0; i < this->size; i++) {
        if(executors[i]->categoryFilter == nullptr || (executors[i]->categoryFilter != nullptr && executors[i]->categoryFilter->id == message->categoryId)) {
            executors[i]->execute(node, message);
        }
    }
}
