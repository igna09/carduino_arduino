#include "DoorNodeEvent.h"

DoorNodeEvent::DoorNodeEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void DoorNodeEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    EventMessage *eventMessage = new EventMessage(message);
    
    if(eventMessage->id == Event::TURN_ON.id) {
        doorCarduinoNode->setup();
    } else if(eventMessage->id == Event::TURN_OFF.id) {
        doorCarduinoNode->setdown();
    }
};
