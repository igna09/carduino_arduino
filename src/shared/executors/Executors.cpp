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
        if(
            executors[i]->categoryFilter == nullptr
            || (
                executors[i]->categoryFilter != nullptr
                && executors[i]->categoryFilter->id == message->categoryId
                && (
                    executors[i]->filterMessage == false
                    || (
                        executors[i]->filterMessage == true
                        && executors[i]->messageId == message->messageId
                    )
                )
            )
        ) {
            executors[i]->execute(node, message);
        }
    }
}
