#include "DoorNodeEvent.h"

DoorNodeEvent::DoorNodeEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void DoorNodeEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    EventMessage *eventMessage = new EventMessage(message);
    
    if(/*eventMessage->id == Event::TURN_ON.id ||*/ eventMessage->id == Event::TURN_OFF_INTERRUPT.id) {
        doorCarduinoNode->setup();
    } else if(eventMessage->id == Event::TURN_OFF.id || eventMessage->id == Event::TURN_ON_INTERRUPT.id) {
        doorCarduinoNode->setdown();
    }

    doorCarduinoNode->lastReceivedEvent = (Event*) eventMessage->event;

    delete eventMessage;
};
