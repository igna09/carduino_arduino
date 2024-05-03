#include "CarduinoNodeCanGetHellos.h"

CarduinoNodeCanGetHellos::CarduinoNodeCanGetHellos() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeCanGetHellos::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    
    if(eventMessage->id == Event::GET_HELLOS.id) {
        EventMessage *helloMessage = new EventMessage(&Event::HELLO, node->id);

        node->sendCanbusMessage(helloMessage);

        delete helloMessage;
    }

    delete eventMessage;
};
