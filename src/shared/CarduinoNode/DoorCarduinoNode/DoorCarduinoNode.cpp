#include "DoorCarduinoNode.h"

DoorCarduinoNode::DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->lowerMirrorsOnReverse = true;
	
	
    StopMovingMirrorsCallback<void(void)>::func = std::bind(&DoorCarduinoNode::stopMoveMirrors, this);
    this->stopMoveMirrorsTask = new Task(MIRRORS_MOVING_TIME, 1, static_cast<TaskCallback>(StopMovingMirrorsCallback<void(void)>::callback), this->scheduler, false);

    BatteryVoltageCallback<void(void)>::func = std::bind(&DoorCarduinoNode::voltageCallback, this);
    temperatureTask = new Task(VOLTAGE_READING_INTERVAL, TASK_FOREVER, static_cast<TaskCallback>(BatteryVoltageCallback<void(void)>::callback), this->scheduler, true);

	this->lastReceivedEvent = nullptr;

	this->canExecutors->addExecutor(new DoorNodeGetSettings());
	this->canExecutors->addExecutor(new DoorNodeWriteSetting());
	this->canExecutors->addExecutor(new DoorNodeEvent());
	this->canExecutors->addExecutor(new DoorNodeCarstatus());

	this->pcfSetup();

	this->closedMirrors = this->readClosedMirrors();
};

void DoorCarduinoNode::loop() {
	CarduinoNode::loop();

	//check if while event TURN_x i receive TURN_x_INTERRUPTED i rollback

	if(this->foldingMirrors) {
		if(this->closedMirrors && !this->readClosedMirrors()) {
			this->closedMirrors = false;
			this->foldingMirrors = false;
		} else if(!this->closedMirrors && this->readClosedMirrors()) {
			this->closedMirrors = true;
			this->foldingMirrors = false;
		}
	}
	if(this->movingMirrors) {

	}
};

void DoorCarduinoNode::setup() {
	if(this->closedMirrors) {
		this->openMirrors();
	}

	//if finished send completed
	// this->sendEvent(&Event::TURN_ON_FINISH);
};

void DoorCarduinoNode::setdown() {
	if(!this->closedMirrors) {
		this->closeMirrors();
	}

	//if finished send completed
	// this->sendEvent(&Event::TURN_OFF_FINISH);
};

void DoorCarduinoNode::startMoveMirrorsDown() {
	digitalWrite(PIN_A, LOW);
	digitalWrite(PIN_B, HIGH);
	digitalWrite(PIN_ENABLE, HIGH);

	this->movingMirrors = true;

	this->stopMoveMirrorsTask->restart();
};

void DoorCarduinoNode::startMoveMirrorsUp() {
	digitalWrite(PIN_A, HIGH);
	digitalWrite(PIN_B, LOW);
	digitalWrite(PIN_ENABLE, HIGH);

	this->movingMirrors = true;

	this->stopMoveMirrorsTask->restart();
};

void DoorCarduinoNode::stopMoveMirrors() {
	digitalWrite(PIN_ENABLE, LOW);

	this->movingMirrors = false;
};

void DoorCarduinoNode::openMirrors() {
	digitalWrite(PIN_OPEN_MIRRORS, HIGH);

	this->foldingMirrors = true;
};

void DoorCarduinoNode::closeMirrors() {
	digitalWrite(PIN_OPEN_MIRRORS, LOW);

	this->foldingMirrors = true;
};

bool DoorCarduinoNode::readClosedMirrors() {
	return digitalRead(PIN_CLOSED_MIRRORS);
};

bool DoorCarduinoNode::usingMirrors() {
	return this->foldingMirrors || this->movingMirrors;
};


void DoorCarduinoNode::pcfSetup() {
    this->pcf8574 = new PCF8574(0x20);

	this->pcf8574->pinMode(PIN_A, OUTPUT);
    this->pcf8574->pinMode(PIN_B, OUTPUT);
    this->pcf8574->pinMode(PIN_ENABLE, OUTPUT);
	this->pcf8574->pinMode(PIN_CLOSED_MIRRORS, INPUT);
    this->pcf8574->pinMode(PIN_OPEN_MIRRORS, OUTPUT);

    this->pcf8574->begin();

    this->pcf8574->digitalWrite(PIN_A, LOW);
    this->pcf8574->digitalWrite(PIN_B, LOW);
    this->pcf8574->digitalWrite(PIN_ENABLE, LOW);
    this->pcf8574->digitalWrite(PIN_OPEN_MIRRORS, LOW);
}



void DoorCarduinoNode::voltageCallback() {
	/**
	 * TODO: fix this calculation
	*/
    float volts = analogRead(VOLTAGE_READING_PIN) * 3.3 / 1024.0;
    
    CarstatusMessage m(&Carstatus::BATTERY_VOLTAGE, volts);
    this->sendCanbusMessage(&m);
};
