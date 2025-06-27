#pragma once

#include "shared/SharedDefinitions.h"
#include "CarduinoNode/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

#include <NimBLEServer.h>

class CommunicationCarduinoNode;
class MyBLEServerCallbacks: public NimBLEServerCallbacks {
    public:
        CommunicationCarduinoNode* node;

        MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode);
        void onConnect(NimBLEServer* pServer, NimBLEConnInfo& desc) override;
        void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& desc, int reason) override;
        void onAuthenticationComplete(const NimBLEConnInfo& connInfo) override;
        uint32_t onPassKeyDisplay() override;
        void onIdentity(const NimBLEConnInfo& connInfo) override;
};