#pragma once

#include "shared/SharedDefinitions.h"
#include "shared/CarduinoNode/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

#include <BLEDevice.h>

class CommunicationCarduinoNode;
class MyBLEServerCallbacks: public BLEServerCallbacks {
    public:
        CommunicationCarduinoNode* node;

        MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode);
        void onConnect(BLEServer* pServer);
        void onDisconnect(BLEServer* pServer);
};