#include "CarduinoNodeCanGetHellos.h"

CarduinoNodeCanGetHellos::CarduinoNodeCanGetHellos() : CarduinoNodeExecutorInterface(&Category::EVENT, Event::GET_HELLOS.id) {};

void CarduinoNodeCanGetHellos::execute(CarduinoNode *node, CanbusMessage *message) {
    // EventMessage *eventMessage = new EventMessage(message);
    // delete eventMessage;

    node->sendLog(10, true);

    EventMessage *helloMessage = new EventMessage(&Event::HELLO, node->id);
    node->sendCanbusMessage(helloMessage);
    delete helloMessage;
};
