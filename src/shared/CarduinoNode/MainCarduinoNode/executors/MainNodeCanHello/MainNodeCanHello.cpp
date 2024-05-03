#include "MainNodeCanHello.h"

MainNodeCanHello::MainNodeCanHello() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainNodeCanHello::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    
    if(eventMessage->id == Event::HELLO.id) {
        NodeInformation *nodeInformation = ((MainCarduinoNode*)node)->createOrGetNodeInformation(eventMessage->getIntValue());

        nodeInformation->lastTimeReceivedHeartBeat = millis();
    }

    delete eventMessage;
};
