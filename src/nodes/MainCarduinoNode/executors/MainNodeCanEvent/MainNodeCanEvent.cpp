#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    // node->printlnWrapper("here " + String(Event::HELLO.id));
    EventMessage *eventMessage = new EventMessage(message);
    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*)node;
    
    if(eventMessage->event->id == Event::HELLO.id) {
        NodeInformation *nodeInformation = mainCarduinoNode->createOrGetNodeInformation(eventMessage->nodeId);
        // mainCarduinoNode->sendLog(1, eventMessage->nodeId);
        // mainCarduinoNode->sendLog(2, nodeInformation->id);

        nodeInformation->lastTimeReceivedHeartBeat = millis();
        nodeInformation->lastCompletedEvent = nullptr;

        node->sendEvent(&Event::ENABLE, eventMessage->nodeId);
    } else if(eventMessage->event->id == Event::BLE_PAIRING_CODE.id) {
        mainCarduinoNode->sendSerialMessage(eventMessage);
    }

    NodeInformation *nodeInformation = mainCarduinoNode->getNodeInformation(eventMessage->nodeId);

    if(nodeInformation != nullptr) {
        nodeInformation->lastCompletedEvent = (Event*) eventMessage->event;
    }

    delete eventMessage;
};

bool MainNodeCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
