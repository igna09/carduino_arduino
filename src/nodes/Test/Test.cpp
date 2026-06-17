#include "Test.h"

Test::Test(uint8_t id, uint8_t cs, uint8_t interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, false, true) {
    this->restoreSettings();

	this->heartbeatWdtTask->disable();
	this->heartbeatTask->disable();
};

void Test::loop() {
	CarduinoNode::loop();
};