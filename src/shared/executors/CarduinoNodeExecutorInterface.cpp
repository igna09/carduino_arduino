#include "CarduinoNodeExecutorInterface.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface() {
    this->filterEvent = false;
    this->eventId = 0;
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(uint8_t eventId) {
    this->eventId = eventId;
    this->filterEvent = true;
}
