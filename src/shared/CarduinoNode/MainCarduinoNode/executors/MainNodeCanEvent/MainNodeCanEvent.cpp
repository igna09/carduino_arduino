#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*)node;
    
    if(eventMessage->event->id == Event::HELLO.id) {
        NodeInformation *nodeInformation = mainCarduinoNode->createOrGetNodeInformation(eventMessage->nodeId);

        nodeInformation->lastTimeReceivedHeartBeat = millis();
        nodeInformation->lastCompletedEvent = nullptr;

        node->sendEvent(&Event::ENABLE, nodeInformation->id);
    }

    NodeInformation *nodeInformation = mainCarduinoNode->getNodeInformation(eventMessage->nodeId);

    if(nodeInformation != nullptr) {
        nodeInformation->lastCompletedEvent = (Event*) eventMessage->event;
    }

    delete eventMessage;
};
