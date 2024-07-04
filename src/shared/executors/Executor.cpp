#include "Executor.h"

void Executor::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void Executor::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper("Executor::execute " + String(message->categoryId) + " " + String(message->messageId));
    if(node->isEnabled || (message->categoryId == Category::EVENT.id && (message->messageId == Event::ENABLE.id || message->messageId == Event::DISABLE.id))) {
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
                    executor->canExecute(node, message)
                    && executor->categoryFilter != nullptr
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
                            node->printlnWrapper("Executor::execute " + specialized->toSerialHumanString());
                            delete specialized;
                        } else {
                            node->printlnWrapper("Executor::execute " + message->toSerialHumanString());
                        }
                    }
                }
                executor->execute(node, message);
            }
        }
    }
}
