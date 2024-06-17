#include "Executor.h"

void Executor::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void Executor::execute(CarduinoNode *node, CanbusMessage *message) {
    if(node->isEnabled) {
        bool logged = false;
        for(uint8_t i = 0; i < this->size; i++) {
            // Serial.print("Executor::execute ");
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
                if(!logged) {
                    logged = true;
                    if(node->_logOnSerial || node->_logOnServer) {
                        Category *category = (Category*)Category::getValueById(message->categoryId);
                        if(category->createSpecializedCopyFunction != nullptr) {
                            CanbusMessage *specialized = category->createSpecializedCopyFunction(message);
                            node->printlnWrapper("CarduinoNode::manageReceivedCanbusMessage " + specialized->toSerialHumanString());
                            delete specialized;
                        } else {
                            node->printlnWrapper("CarduinoNode::manageReceivedCanbusMessage " + message->toSerialHumanString());
                        }
                    }
                }
                executor->execute(node, message);
            }
        }
    }
}
