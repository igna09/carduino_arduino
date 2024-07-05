#pragma once

#include "shared/SharedDefinitions.h"
#include "shared/CarduinoNode/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

#include <NimBLEServer.h>

class CommunicationCarduinoNode;
class MyBLEServerCallbacks: public NimBLEServerCallbacks {
    public:
        CommunicationCarduinoNode* node;

        MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode);
        void onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
        void onDisconnect(NimBLEServer* pServer, ble_gap_conn_desc* desc);
        void onAuthenticationComplete(ble_gap_conn_desc* desc);
        uint32_t onPassKeyRequest();
};