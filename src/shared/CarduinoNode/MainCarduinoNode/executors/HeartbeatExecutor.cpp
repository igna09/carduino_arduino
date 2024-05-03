#include "HeartbeatExecutor.h"

HeartbeatExecutor::HeartbeatExecutor() : CarduinoNodeExecutorInterface(&Category::HEARTBEAT) {};

void HeartbeatExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    HeartbeatMessage *heartbeatMessage = new HeartbeatMessage(message);
    
    /**
     * get from node last time received heartbeat message from sender
    */
    NodeInformation *nodeInformation = ((MainCarduinoNode*)node)->getNodeInformation(heartbeatMessage->senderId);
    if(nodeInformation != nullptr) {
        if(nodeInformation->lastTimeReceivedHeartBeat != 0 && millis() - nodeInformation->lastTimeReceivedHeartBeat > HEARTBEAT_INTERVAL + HEARTBEAT_INTERVAL_TOLERANCE) { // ERROR
            //TODO: error management
            node->printlnWrapper("----- HEARTBEAT ERROR -----");
        } else { // NO ERROR or first heartbeat
            nodeInformation->lastTimeReceivedHeartBeat = millis();
        }
    }

    delete heartbeatMessage;
};
