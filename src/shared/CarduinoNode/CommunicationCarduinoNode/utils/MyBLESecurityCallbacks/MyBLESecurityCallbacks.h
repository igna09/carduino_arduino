#pragma once

#include "shared/SharedDefinitions.h"
#include "shared/CarduinoNode/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

#include <BLEDevice.h>

class CommunicationCarduinoNode; 
class MyBLESecurityCallbacks : public BLESecurityCallbacks {
    public:
        CommunicationCarduinoNode* node;

        MyBLESecurityCallbacks(CommunicationCarduinoNode* node);
        uint32_t onPassKeyRequest();
        void onPassKeyNotify(uint32_t pass_key);
        bool onConfirmPIN(uint32_t pass_key);
        bool onSecurityRequest();
        void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl);
};