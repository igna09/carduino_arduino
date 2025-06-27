
#include "MyCharacteristicCallbacks.h"

MyCharacteristicCallbacks::MyCharacteristicCallbacks(CommunicationCarduinoNode* carduinoNode) {
    this->node = carduinoNode;
}

void MyCharacteristicCallbacks::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo)  {
    node->printlnWrapper("MyCharacteristicCallbacks::onWrite " + String(pCharacteristic->getValue().c_str()));
}
