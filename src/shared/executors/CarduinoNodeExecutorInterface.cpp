#include "CarduinoNodeExecutorInterface.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface() {
    this->filterEvent = false;
    this->eventEnum = nullptr;
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(const EventEnum* eventEnum) {
    this->eventEnum = eventEnum;
    this->filterEvent = true;
}
