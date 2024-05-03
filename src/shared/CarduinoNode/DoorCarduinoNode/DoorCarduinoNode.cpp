#include "DoorCarduinoNode.h"

DoorCarduinoNode::DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->lowerMirrorsOnReverse = true;
	
	this->canExecutors->addExecutor(new DoorNodeReadSetting());
	this->canExecutors->addExecutor(new DoorNodeWriteSetting());
};

void DoorCarduinoNode::loop() {
	CarduinoNode::loop();
};
