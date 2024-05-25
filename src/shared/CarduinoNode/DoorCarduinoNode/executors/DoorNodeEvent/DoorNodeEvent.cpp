#include "DoorNodeEvent.h"

DoorNodeEvent::DoorNodeEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void DoorNodeEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    EventMessage *eventMessage = new EventMessage(message);

    // doorCarduinoNode->printlnWrapper("received event " + String(eventMessage->event->name));
    
    if(/*eventMessage->event->id == Event::TURN_ON.id ||*/ eventMessage->event->id == Event::TURN_OFF_INTERRUPT.id) {
        doorCarduinoNode->openMirrors();
    } else if(eventMessage->event->id == Event::TURN_OFF.id || eventMessage->event->id == Event::TURN_ON_INTERRUPT.id) {
        doorCarduinoNode->setdown();
    }

    doorCarduinoNode->lastReceivedEvent = (Event*) eventMessage->event;

    delete eventMessage;
};
