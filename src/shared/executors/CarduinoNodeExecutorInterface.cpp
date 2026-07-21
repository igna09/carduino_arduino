#include "CarduinoNodeExecutorInterface.h"
#include "EventRegistry.h"

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface() {
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(uint8_t eventId): hasEvents(true) {
    this->eventIds.push_back(eventId);
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(std::initializer_list<uint8_t> eventIds)
    : eventIds(eventIds), hasEvents(true) {
}

CarduinoNodeExecutorInterface::CarduinoNodeExecutorInterface(EventCategory category)
    : category(category), hasCategory(true) {
}

bool CarduinoNodeExecutorInterface::matchesEvent(uint8_t eventId) const {
    if (this->hasCategory) {
        return EventRegistry::getCategory(eventId) == this->category;
    }
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
    return true;
}

bool CarduinoNodeExecutorInterface::isGeneric() {
    return !hasCategory && !hasEvents;
}