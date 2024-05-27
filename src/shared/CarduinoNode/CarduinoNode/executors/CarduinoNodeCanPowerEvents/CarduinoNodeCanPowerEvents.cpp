#include "CarduinoNodeCanPowerEvents.h"

CarduinoNodeCanPowerEvents::CarduinoNodeCanPowerEvents() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeCanPowerEvents::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);

    if(eventMessage->getIntValue() == node->id) {
        if(eventMessage->event->id == Event::TURN_ON.id) {
            node->turnOn();
        } else if(eventMessage->event->id == Event::TURN_OFF.id) {
            node->turnOff();
        } else if(eventMessage->event->id == Event::TURN_ON_INTERRUPT.id) {
            node->turnOnInterrupt();
        } else if(eventMessage->event->id == Event::TURN_OFF_INTERRUPT.id) {
            node->turnOffInterrupt();
        }
    }

    delete eventMessage;
};
