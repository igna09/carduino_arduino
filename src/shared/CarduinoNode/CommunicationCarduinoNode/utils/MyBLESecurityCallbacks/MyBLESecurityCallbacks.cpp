
#include "MyBLESecurityCallbacks.h"

MyBLESecurityCallbacks::MyBLESecurityCallbacks(CommunicationCarduinoNode* node) {
    this->node = node;
}

uint32_t MyBLESecurityCallbacks::onPassKeyRequest() {
    node->printlnWrapper("MyBLESecurityCallbacks::PassKeyRequest");
    return BLE_PIN;
}

void MyBLESecurityCallbacks::onPassKeyNotify(uint32_t pass_key) {
    node->printlnWrapper("MyBLESecurityCallbacks:: The passkey Notify number: " + String(pass_key));
}

bool MyBLESecurityCallbacks::onConfirmPIN(uint32_t pass_key) {
    node->printlnWrapper("MyBLESecurityCallbacks:: The passkey YES/NO number: " + String(pass_key));
    return true;
}

bool MyBLESecurityCallbacks::onSecurityRequest() {
    node->printlnWrapper("MyBLESecurityCallbacks::onSecurityRequest");
    return true;
}

void MyBLESecurityCallbacks::onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
    if(cmpl.success){
        node->printlnWrapper("MyBLESecurityCallbacks::onAuthenticationComplete success");
        uint16_t length;
        esp_ble_gap_get_whitelist_size(&length);
        Serial.print("whitelist size: ");
        Serial.println(length);

        node->clientAuthenticated();
    } else {
        node->printlnWrapper("MyBLESecurityCallbacks::onAuthenticationComplete error");
    }
}