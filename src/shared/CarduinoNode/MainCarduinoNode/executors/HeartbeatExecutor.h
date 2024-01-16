#pragma once

#include <Arduino.h>
#include "../../../CanbusMessage/CanbusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"
#include "../../MainCarduinoNode/MainCarduinoNode.h"
#include "../../../executors/CarduinoNodeExecutorInterface.h"
#include "../../../enums/Setting.h"
#include "../../../CanbusMessage/HeartbeatMessage/HeartbeatMessage.h"
#include "../../../enums/Category.h"

class HeartbeatExecutor : public CarduinoNodeExecutorInterface {
    public:
        HeartbeatExecutor() : CarduinoNodeExecutorInterface(&Category::HEARTBEAT) {};

        void execute(CarduinoNode *node, CanbusMessage *message) {
            HeartbeatMessage *heartbeatMessage = new HeartbeatMessage(*message);
            
            /**
             * get from node last time received heartbeat message from sender
            */
            std::map<uint8_t,unsigned long> map = ((MainCarduinoNode*)node)->lastReceivedHeartbeat;
            std::map<uint8_t,unsigned long>::iterator it = map.find(heartbeatMessage->senderId);
            if (it != map.end()) {
                if(millis() - it->second > HEARTBEAT_INTERVAL + HEARTBEAT_INTERVAL_TOLERANCE) { // ERROR
                    //TODO: error management
                } else { // NO ERROR
                    map[heartbeatMessage->senderId] = millis();
                }
            } else { // first heartbeat ever (improve this management because if wont boot it is ok)
                // map.insert( std::pair<uint8_t,unsigned long>(heartbeatMessage->senderId, millis()) );
                map[heartbeatMessage->senderId] = millis();
            }

            delete heartbeatMessage;
        };
};
