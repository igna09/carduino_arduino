#include "DoorCarduinoNode.h"

DoorCarduinoNode::DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->addSetting(&Setting::AUTO_CLOSE_REARVIEW_MIRRORS, true);
    this->addSetting(&Setting::ON_REVERSE_LOWER_MIRRORS, true);
	
    this->stopMoveMirrorsTask = new Task(MIRRORS_MOVING_TIME, 1, std::bind(&DoorCarduinoNode::stopMoveMirrors, this), this->scheduler, false);

    temperatureTask = new Task(VOLTAGE_READING_INTERVAL, TASK_FOREVER, std::bind(&DoorCarduinoNode::voltageCallback, this), this->scheduler, true);

	this->lastReceivedEvent = nullptr;

	this->canExecutors->addExecutor(new DoorNodeEvent());
	this->canExecutors->addExecutor(new DoorNodeCarstatus());

	this->pcfSetup();

	// this->closedMirrors = this->readClosedMirrors();
	this->mirrorSelectorOnClosed = this->readSelectorClosed();
};

void DoorCarduinoNode::loop() {
	CarduinoNode::loop();

	//check if while event TURN_x i receive TURN_x_INTERRUPTED i rollback

	// if(this->foldingMirrors) {
	// 	if(this->closedMirrors && !this->readClosedMirrors()) {
	// 		this->closedMirrors = false;
	// 		this->foldingMirrors = false;
	// 	} else if(!this->closedMirrors && this->readClosedMirrors()) {
	// 		this->closedMirrors = true;
	// 		this->foldingMirrors = false;
	// 	}
	// }
	
	if(this->movingMirrors) {

	}

	// if(this->mirrorSelectorOnClosed != this->readSelectorClosed()) {
	// 	this->mirrorSelectorOnClosed = this->readSelectorClosed();
	// }
};

void DoorCarduinoNode::startMoveMirrorsDown() {
	printlnWrapper("DoorCarduinoNode::startMoveMirrorsDown");
	this->pcf8574->digitalWrite(PIN_MIRROR_A, HIGH);
	this->pcf8574->digitalWrite(PIN_MIRROR_B, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_C, HIGH);
	this->pcf8574->digitalWrite(PIN_MIRROR_D, LOW);

	this->movingMirrors = true;

	this->stopMoveMirrorsTask->restartDelayed();
};

void DoorCarduinoNode::startMoveMirrorsUp() {
	printlnWrapper("DoorCarduinoNode::startMoveMirrorsUp");
	this->pcf8574->digitalWrite(PIN_MIRROR_A, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_B, HIGH);
	this->pcf8574->digitalWrite(PIN_MIRROR_C, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_D, HIGH);

	this->movingMirrors = true;

	this->stopMoveMirrorsTask->restartDelayed();
};

void DoorCarduinoNode::stopMoveMirrors() {
	printlnWrapper("DoorCarduinoNode::stopMoveMirrors");
	this->pcf8574->digitalWrite(PIN_MIRROR_A, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_B, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_C, LOW);
	this->pcf8574->digitalWrite(PIN_MIRROR_D, LOW);

	this->movingMirrors = false;
};

void DoorCarduinoNode::openMirrors() {
	printlnWrapper("DoorCarduinoNode::openMirrors");
	if(this->getSettingValue(&Setting::AUTO_CLOSE_REARVIEW_MIRRORS)->valueType->boolValue) {
		printlnWrapper("DoorCarduinoNode::openMirrors opening");
		this->pcf8574->digitalWrite(PIN_OPEN_MIRRORS, LOW);
	}
};

void DoorCarduinoNode::closeMirrors() {
	printlnWrapper("DoorCarduinoNode::closeMirrors");
	if(this->getSettingValue(&Setting::AUTO_CLOSE_REARVIEW_MIRRORS)->valueType->boolValue) {
		printlnWrapper("DoorCarduinoNode::openMirrors closing");
		this->pcf8574->digitalWrite(PIN_OPEN_MIRRORS, HIGH);
	}
};

bool DoorCarduinoNode::readClosedMirrors() {
	printlnWrapper("DoorCarduinoNode::readClosedMirrors");
	return this->pcf8574->digitalRead(PIN_CLOSED_MIRRORS_RELAY, true) == HIGH;
};

bool DoorCarduinoNode::usingMirrors() {
	return this->movingMirrors;
};


void DoorCarduinoNode::pcfSetup() {
    this->pcf8574 = new PCF8574(0x20);

	this->pcf8574->pinMode(PIN_MIRROR_A, OUTPUT);
    this->pcf8574->pinMode(PIN_MIRROR_B, OUTPUT);
	this->pcf8574->pinMode(PIN_MIRROR_C, OUTPUT);
    this->pcf8574->pinMode(PIN_MIRROR_D, OUTPUT);
	this->pcf8574->pinMode(PIN_CLOSED_MIRRORS_RELAY, INPUT);
    this->pcf8574->pinMode(PIN_OPEN_MIRRORS, OUTPUT);
    this->pcf8574->pinMode(PIN_MIRROR_SELECTOR_ON_CLOSED, INPUT);

    this->pcf8574->begin();

    this->pcf8574->digitalWrite(PIN_MIRROR_A, LOW);
    this->pcf8574->digitalWrite(PIN_MIRROR_B, LOW);
    this->pcf8574->digitalWrite(PIN_MIRROR_C, LOW);
    this->pcf8574->digitalWrite(PIN_MIRROR_D, LOW);
    this->pcf8574->digitalWrite(PIN_OPEN_MIRRORS, LOW);

	this->addPinToRead(PIN_MIRROR_SELECTOR_ON_CLOSED, this->pcf8574, [&](PinInformation *pinInformation){
		printlnWrapper("PIN_MIRROR_SELECTOR_ON_CLOSED changed from " + String(!pinInformation->isHigh) + " to " + String(pinInformation->isHigh));
		this->mirrorSelectorOnClosed = pinInformation->isHigh;

		// if(this->mirrorSelectorOnClosed) {
		// 	this->closeMirrors();
		// } else {
		// 	this->openMirrors();
		// }
	});
}

void DoorCarduinoNode::voltageCallback() {
	int read = analogRead(VOLTAGE_READING_PIN);
    float tension1voltReference = read / 1024.0; // voltage on esp8266 adc pin
	// float volts = tension1voltReference * 5.4; // max Vin = 5.4 = vOut * ((R1 + R2) / R2) = 1 * (((220k + 220k) + 100k) / 100k) ---- vOut = maxVOut = 1 (max input voltage on esp8266 adc)
	// float volts = tension1voltReference * (((WEMOS_D1_MINI_VOLTAGE_DIVIDER_R1 + VOLTAGE_READING_PIN_RESISTOR) + WEMOS_D1_MINI_VOLTAGE_DIVIDER_R2) / WEMOS_D1_MINI_VOLTAGE_DIVIDER_R2);
	float volts = calculateVoltage(tension1voltReference, WEMOS_D1_MINI_VOLTAGE_DIVIDER_R1 + VOLTAGE_READING_PIN_RESISTOR, WEMOS_D1_MINI_VOLTAGE_DIVIDER_R2) * VOLTAGE_READING_PIN_COMPENSATION;
    
    CarstatusMessage m(&Carstatus::BATTERY_VOLTAGE, volts);
    this->sendCanbusMessage(&m);
};

bool DoorCarduinoNode::readSelectorClosed() {
	return this->pcf8574->digitalRead(PIN_MIRROR_SELECTOR_ON_CLOSED, true) == HIGH;
}

void DoorCarduinoNode::turnOn() {
	CarduinoNode::turnOn();
	printlnWrapper("DoorCarduinoNode::turnOn");

	delayTask(1500, [&](){
		if(!this->mirrorSelectorOnClosed) {
			this->openMirrors();
		}
	});
}

void DoorCarduinoNode::turnOff() {
	CarduinoNode::turnOff();
	printlnWrapper("DoorCarduinoNode::turnOff");

	delayTask(1500, [&](){
		if(!this->mirrorSelectorOnClosed) {
			this->closeMirrors();
		}
	});
}

void DoorCarduinoNode::turnOffInterrupt() {
	CarduinoNode::turnOffInterrupt();
	printlnWrapper("DoorCarduinoNode::turnOffInterrupt");

	if(!this->mirrorSelectorOnClosed) {
		this->openMirrors();
	}
}

void DoorCarduinoNode::turnOnInterrupt() {
	CarduinoNode::turnOnInterrupt();
	printlnWrapper("DoorCarduinoNode::turnOnInterrupt");

	if(!this->mirrorSelectorOnClosed) {
		this->closeMirrors();
	}
}
