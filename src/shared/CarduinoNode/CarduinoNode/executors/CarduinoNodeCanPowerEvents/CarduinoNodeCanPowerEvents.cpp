#include "CarduinoNodeCanPowerEvents.h"

CarduinoNodeCanPowerEvents::CarduinoNodeCanPowerEvents() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeCanPowerEvents::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    node->printlnWrapper("CarduinoNodeCanPowerEvents::execute");
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
        }
    }

    delete eventMessage;
};
