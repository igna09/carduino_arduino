#include "CarduinoNodeExecutorInterface.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface() {
    // eventIds resta vuoto: nessun filtro.
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(uint8_t eventId) {
    this->eventIds.push_back(eventId);
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(std::initializer_list<uint8_t> eventIds)
    : eventIds(eventIds) {
}

bool CarduinoNodeExecutorInterface::matchesEvent(uint8_t eventId) const {
    if (this->eventIds.empty()) {
        return true; // nessun filtro: passa sempre
    }
    for (uint8_t id : this->eventIds) {
        if (id == eventId) {
            return true;
        }
    }
    return false;
}

bool CarduinoNodeExecutorInterface::canExecute(CarduinoNode *node, Message *message) {
    // Default implementation: sempre true. Override in derived classes se
    // serve logica più complessa.
    return true;
}