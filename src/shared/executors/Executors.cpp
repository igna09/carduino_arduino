#include "Executors.h"

void Executors::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void Executors::execute(CarduinoNode *node, CanbusMessage *message) {
    for(uint8_t i = 0; i < this->size; i++) {
        // Serial.print("Executors::execute ");
        //     Serial.print(executors[i]->categoryFilter->name);
        //     Serial.print(" ");
        //     Serial.print(executors[i]->filterMessage ? "TRUE" : "FALSE");
        //     Serial.print(" ");
        //     Serial.println(executors[i]->messageId);
        CarduinoNodeExecutorInterface *executor = executors[i];
        if(
            executor->categoryFilter == nullptr
            || (
                executor->categoryFilter != nullptr
                && executor->categoryFilter->id == message->categoryId
                && (
                    executor->filterMessage == false
                    || (
                        executor->filterMessage == true
                        && executor->messageId == message->messageId
                    )
                )
            )
        ) {
            executor->execute(node, message);
        }
    }
}
