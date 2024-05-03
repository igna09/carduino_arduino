#include "DoorCarduinoNode.h"

DoorCarduinoNode::DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->lowerMirrorsOnReverse = true;
	
	this->canExecutors->addExecutor(new DoorNodeGetSettings());
	this->canExecutors->addExecutor(new DoorNodeWriteSetting());
};

void DoorCarduinoNode::loop() {
	CarduinoNode::loop();

	//check if while event TURN_x i receive TURN_x_INTERRUPTED i rollback
};

void DoorCarduinoNode::setup() {
	// check if mirror is closed, if yes i open it

	//if finished send completed
	// this->sendEvent(&Event::TURN_ON_FINISH);
};

void DoorCarduinoNode::setdown() {
	//i close mirrors

	//if finished send completed
	// this->sendEvent(&Event::TURN_OFF_FINISH);
};
