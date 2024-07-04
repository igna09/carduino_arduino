#include "HeartbeatExecutor.h"

HeartbeatExecutor::HeartbeatExecutor() : CarduinoNodeExecutorInterface(&Category::EVENT, Event::HEARTBEAT.id) {};

void HeartbeatExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    
    /**
     * get from node last time received heartbeat message from sender
    */
    NodeInformation *nodeInformation = ((MainCarduinoNode*)node)->getNodeInformation(eventMessage->nodeId);
    if(nodeInformation != nullptr) {
        if(nodeInformation->lastTimeReceivedHeartBeat != 0 && millis() - nodeInformation->lastTimeReceivedHeartBeat > HEARTBEAT_INTERVAL + HEARTBEAT_INTERVAL_TOLERANCE) { // ERROR
            //TODO: error management
            node->printlnWrapper("----- HEARTBEAT ERROR -----");
        } else { // NO ERROR or first heartbeat
            nodeInformation->lastTimeReceivedHeartBeat = millis();
        }
    }

    delete eventMessage;
};

bool HeartbeatExecutor::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled;
}
