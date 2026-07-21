#include "Executor.h"

void Executor::addExecutor(CarduinoNodeExecutorInterface* executor) {
    this->executors.push_back(executor);
}

void Executor::execute(CarduinoNode *node, Message *message) {
    for (CarduinoNodeExecutorInterface *executor : this->executors) {
        if (executor->matchesEvent(message->event->id) && executor->canExecute(node, message)) {
            executor->execute(node, message);
        }
    }
}

bool Executor::willExecute(CarduinoNode *node, Message *message) {
    for (CarduinoNodeExecutorInterface *executor : this->executors) {
        if (!executor->isGeneric() && executor->matchesEvent(message->event->id)) {
            return true;
        }
    }
    return false;
}