#pragma once

#include "shared/SharedDefinitions.h"
#include "shared/CarduinoNode/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

#include <NimBLEServer.h>

class CommunicationCarduinoNode;
class MyCharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    public:
        CommunicationCarduinoNode* node;

        MyCharacteristicCallbacks(CommunicationCarduinoNode* carduinoNode);
        void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
};