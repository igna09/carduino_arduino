#include "CarduinoNodeCanEvent.h"

CarduinoNodeCanEvent::CarduinoNodeCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    node->printlnWrapper("CarduinoNodeCanEvent::execute");
    node->sendLog(3, eventMessage->getIntValue());
    node->sendLog(3, eventMessage->nodeId);
    node->sendLog(3, node->id);
    node->sendLog(3, eventMessage->event->id);
    node->sendLog(30, true);

    if(eventMessage->getIntValue() == node->id || eventMessage->getIntValue() == ALL_NODES) {
        if(eventMessage->event->id == Event::ENABLE.id) {
            node->enable();
        } else if(eventMessage->event->id == Event::DISABLE.id) {
            node->disable();
        } else if(eventMessage->event->id == Event::ENABLE_INTERRUPT.id) {
            node->enableInterrupt();
        } else if(eventMessage->event->id == Event::DISABLE_INTERRUPT.id) {
            node->disableInterrupt();
        } else if(eventMessage->event->id == Event::RESET_WEBAPP.id) {
            node->resetWebapp();
        }

    }

    delete eventMessage;
};
