#include "Executor.h"

void Executor::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors.push_back(executor);
}

void Executor::execute(CarduinoNode *node, Message *message) {
    for (CarduinoNodeExecutorInterface *executor : this->executors) {
        if (executor->matchesEvent(message->event->id) && executor->canExecute(node, message)) {
            NLOGI("received message %s", message->toString().c_str());
            executor->execute(node, message);
        }
    }
}