
#include "MyBLEServerCallbacks.h"

MyBLEServerCallbacks::MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode) {
    this->node = carduinoNode;
}

void MyBLEServerCallbacks::onConnect(BLEServer* pServer) {
    node->printlnWrapper("MyBLEServerCallbacks::onConnect");
    
    node->connected = true;
}

void MyBLEServerCallbacks::onDisconnect(BLEServer* pServer) {
    node->printlnWrapper("MyBLEServerCallbacks::onDisconnect");

    node->connected = false;
    node->authenticated = false;
    delete node->authenticatedBdAddress;

    pServer->getAdvertising()->start();
    node->printlnWrapper("MyBLEServerCallbacks::onDisconnect Waiting a new client connection to notify...");
}