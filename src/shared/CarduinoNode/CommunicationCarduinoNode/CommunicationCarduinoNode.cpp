#include "CommunicationCarduinoNode.h"

CommunicationCarduinoNode::CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->restoreSettings();
};

void CommunicationCarduinoNode::loop() {
    CarduinoNode::loop();
}
