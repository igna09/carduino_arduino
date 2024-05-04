#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    
    if(eventMessage->id == Event::HELLO.id) {
        NodeInformation *nodeInformation = ((MainCarduinoNode*)node)->createOrGetNodeInformation(eventMessage->senderId);

        nodeInformation->lastTimeReceivedHeartBeat = millis();
    } else if(eventMessage->id == Event::TURN_OFF_FINISH.id || eventMessage->id == Event::TURN_ON_FINISH.id) {
        NodeInformation *nodeInformation = ((MainCarduinoNode*)node)->getNodeInformation(eventMessage->senderId);

        if(nodeInformation != nullptr) {
            nodeInformation->lastCompletedEvent = (Event*) Event::getValueById(eventMessage->id);
        }
    }

    delete eventMessage;
};
