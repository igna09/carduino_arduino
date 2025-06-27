#include "Sniffer.h"

Sniffer::Sniffer(uint8_t id, uint8_t cs, uint8_t interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, false, true) {
    this->restoreSettings();

	this->canExecutor->addExecutor(new AllCanEvent());

	this->heartbeatWdtTask->disable();
	this->heartbeatTask->disable();
};

void Sniffer::loop() {
	CarduinoNode::loop();
};