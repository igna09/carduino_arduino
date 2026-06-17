#include "Executor.h"

void Executor::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors[this->size] = executor;
    this->size++;
};

void Executor::execute(CarduinoNode *node, CanbusMessage *message) {
    // node->printlnWrapper("Executor::execute " + String(message->categoryId) + " " + String(message->messageId));
    //if(node->isEnabled || (message->categoryId == Category::EVENT.id && (message->messageId == Event::ENABLE.id || message->messageId == Event::DISABLE.id || message->messageId == Event::HELLO.id || message->messageId == Event::GET_HELLOS.id))) {
        bool logged = false;
        for(uint8_t i = 0; i < this->size; i++) {
            CarduinoNodeExecutorInterface *executor = executors[i];
            if(
                (
                    executor->filterEvent == false
                    || (
                        executor->filterEvent == true
                        && executor->eventId == message->event->id
                    )
                )
                && executor->canExecute(node, message)
            ) {
                // if(!logged) {
                //     logged = true;
                //     if(node->_logOnSerial || node->_logOnServer) {
                //         Category *category = (Category*)Category::getValueById(message->categoryId);
                //         if(category->createSpecializedCopyFunction != nullptr) {
                //             CanbusMessage *specialized = category->createSpecializedCopyFunction(message);
                //             node->printlnWrapper("Executor::execute executing " + specialized->toSerialHumanString());
                //             delete specialized;
                //         } else {
                //             node->printlnWrapper("Executor::execute executing " + message->toSerialHumanString());
                //         }
                //     }
                // }
                executor->execute(node, message);
            }
        }
    //}
}
