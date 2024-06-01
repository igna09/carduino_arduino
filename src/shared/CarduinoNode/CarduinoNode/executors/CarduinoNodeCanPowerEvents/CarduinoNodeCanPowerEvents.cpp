#include "CarduinoNodeCanPowerEvents.h"

CarduinoNodeCanPowerEvents::CarduinoNodeCanPowerEvents() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeCanPowerEvents::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    node->printlnWrapper("CarduinoNodeCanPowerEvents::execute");

    if(eventMessage->getIntValue() == node->id || eventMessage->getIntValue() == ALL_NODES) {
        if(eventMessage->event->id == Event::ENABLE.id) {
            node->enable();
        } else if(eventMessage->event->id == Event::DISABLE.id) {
            node->disable();
        } else if(eventMessage->event->id == Event::ENABLE_INTERRUPT.id) {
            node->enableInterrupt();
        } else if(eventMessage->event->id == Event::DISABLE_INTERRUPT.id) {
            node->disableInterrupt();
        }
    }

    delete eventMessage;
};
