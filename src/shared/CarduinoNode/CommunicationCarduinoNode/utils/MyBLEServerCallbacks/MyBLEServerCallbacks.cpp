
#include "MyBLEServerCallbacks.h"

MyBLEServerCallbacks::MyBLEServerCallbacks(CommunicationCarduinoNode* carduinoNode) {
    this->node = carduinoNode;
}

void MyBLEServerCallbacks::onConnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
    node->printlnWrapper("MyBLEServerCallbacks::onConnect");

    NimBLEConnInfo info = pServer->getPeerIDInfo(desc->conn_handle);
    NimBLEAddress clientAddress(info.getAddress());
    if(node->disabledPairing && !NimBLEDevice::onWhiteList(clientAddress)) {
        pServer->disconnect(info.getConnHandle());
    }
}

void MyBLEServerCallbacks::onDisconnect(NimBLEServer* pServer, ble_gap_conn_desc* desc) {
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

void MyBLEServerCallbacks::onAuthenticationComplete(ble_gap_conn_desc* desc) {
    node->printlnWrapper("MyBLEServerCallbacks::onAuthenticationComplete success");

    node->clientAuthenticated(desc);
}

uint32_t MyBLEServerCallbacks::onPassKeyRequest() {
    String pinString = String(esp_random());
    pinString = pinString.substring(0, 6);
    node->printlnWrapper("MyBLEServerCallbacks::onPassKeyRequest " + pinString);

    int pin = pinString.toInt();
    node->sendEvent(&Event::BLE_PAIRING_CODE, pin);

    return pin;
}
