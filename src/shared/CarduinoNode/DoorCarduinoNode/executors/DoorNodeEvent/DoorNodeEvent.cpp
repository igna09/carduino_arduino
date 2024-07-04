#include "DoorNodeEvent.h"

DoorNodeEvent::DoorNodeEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void DoorNodeEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    EventMessage *eventMessage = new EventMessage(message);

    // doorCarduinoNode->printlnWrapper("received event " + String(eventMessage->event->name));
    
    // if(/*eventMessage->event->id == Event::ENABLE.id ||*/ eventMessage->event->id == Event::DISABLE_INTERRUPT.id) {
    //     doorCarduinoNode->openMirrors();
    // } else if(eventMessage->event->id == Event::DISABLE.id || eventMessage->event->id == Event::ENABLE_INTERRUPT.id) {
    //     doorCarduinoNode->setdown();
    // }

    doorCarduinoNode->lastReceivedEvent = (Event*) eventMessage->event;

    delete eventMessage;
};

bool DoorNodeEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
