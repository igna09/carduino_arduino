
#include "MyBLEServerCallbacks.h"

MyBLEServerCallbacks::MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode) {
    this->node = carduinoNode;
}

void MyBLEServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& desc) {
    node->printlnWrapper("MyBLEServerCallbacks::onConnect");
    
    String message = "onConnect [bd_addr: ";
    message += desc.getAddress().toString().c_str();
    message += ", success: ";
    message += desc.isAuthenticated();
    message += ", bonded: ";
    message += desc.isBonded();
    message += ", encrypted: ";
    message += desc.isEncrypted();
    message += "]";
    node->printlnWrapper(message, false);
}

void MyBLEServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& desc, int reason) {
    node->printlnWrapper("MyBLEServerCallbacks::onDisconnect Waiting a new client connection to notify...");
    pServer->getAdvertising()->start();
    node->rssiTask->disable();
}

void MyBLEServerCallbacks::onAuthenticationComplete(const NimBLEConnInfo& desc) {
    node->printlnWrapper("MyBLEServerCallbacks::onAuthenticationComplete " + String(desc.getIdAddress().toString().c_str()));

    node->clientAuthenticated(desc);
}

uint32_t MyBLEServerCallbacks::onPassKeyDisplay() {
    String pinString = String(esp_random());
    pinString = pinString.substring(0, 6);
    node->printlnWrapper("MyBLEServerCallbacks::onPassKeyRequest " + pinString);

    int pin = pinString.toInt();
    node->sendEvent(&Event::BLE_PAIRING_CODE, pin);

    return pin;
}

void MyBLEServerCallbacks::onIdentity(const NimBLEConnInfo& desc) {
    node->printlnWrapper("MyBLEServerCallbacks::onIdentity");
    node->printlnWrapper("MyBLEServerCallbacks::onIdentity identity address " + String(desc.getIdAddress().toString().c_str()));

    node->onIdentity(desc);
}
