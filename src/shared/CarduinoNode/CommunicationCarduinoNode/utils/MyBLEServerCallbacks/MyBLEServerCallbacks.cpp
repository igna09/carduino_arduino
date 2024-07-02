
#include "MyBLEServerCallbacks.h"

MyBLEServerCallbacks::MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode) {
    this->node = carduinoNode;
}

void MyBLEServerCallbacks::onConnect(BLEServer* pServer) {
    node->printlnWrapper("MyBLEServerCallbacks::onConnect");
}

void MyBLEServerCallbacks::onDisconnect(BLEServer* pServer) {
    node->printlnWrapper("MyBLEServerCallbacks::onDisconnect");

    node->rssiTask->disable();
    if(node->authenticatedBdAddress != nullptr) {
        delete node->authenticatedBdAddress;
        node->authenticatedBdAddress = nullptr;
    }

    if(!node->disabledPairing) {
        pServer->getAdvertising()->start();
    }
    node->printlnWrapper("MyBLEServerCallbacks::onDisconnect Waiting a new client connection to notify...");
}