#include "HeartbeatExecutor.h"

HeartbeatExecutor::HeartbeatExecutor() : CarduinoNodeExecutorInterface(&Category::HEARTBEAT) {};

void HeartbeatExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    HeartbeatMessage *heartbeatMessage = new HeartbeatMessage(message);
    
    /**
     * get from node last time received heartbeat message from sender
    */
    std::map<uint8_t,unsigned long> *map = ((MainCarduinoNode*)node)->lastReceivedHeartbeats;
    std::map<uint8_t,unsigned long>::iterator it = map->find(heartbeatMessage->senderId);
    if (it != map->end()) {
        if(millis() - it->second > HEARTBEAT_INTERVAL + HEARTBEAT_INTERVAL_TOLERANCE) { // ERROR
            //TODO: error management
            node->printlnWrapper("----- HEARTBEAT ERROR -----");
        } else { // NO ERROR
            (*map)[heartbeatMessage->senderId] = millis();
        }
    } else { // first heartbeat ever (improve this management because if wont boot it is ok)
        (*map)[heartbeatMessage->senderId] = millis();
    }

    delete heartbeatMessage;
};
