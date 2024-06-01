#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*)node;
    
    if(eventMessage->event->id == Event::HELLO.id) {
        NodeInformation *nodeInformation = mainCarduinoNode->createOrGetNodeInformation(eventMessage->senderReceiverId);

        nodeInformation->lastTimeReceivedHeartBeat = millis();
        nodeInformation->lastCompletedEvent = nullptr;

        node->sendEvent(&Event::ENABLE, nodeInformation->id);
    } else if(eventMessage->event->id == Event::DISABLE_FINISH.id || eventMessage->event->id == Event::ENABLE_FINISH.id) {
        NodeInformation *nodeInformation = mainCarduinoNode->getNodeInformation(eventMessage->senderReceiverId);

        if(nodeInformation != nullptr) {
            nodeInformation->lastCompletedEvent = (Event*) eventMessage->event;
        }
    }

    delete eventMessage;
};
