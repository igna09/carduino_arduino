#include "MediaControlCarduinoNode.h"

/**
 * TODO: 
 * 	TEST taskscheduler che riporta la resistenza a 0 dopo tot di ms (da recuperare da main node)
 * 	TEST portare la registrazione dei pulsanti da main node a qui
 * 	durante la registrazione dei pulsanti mandare messaggio alla radio (non visibile perché siamo su app radio swc, BLE_PAIRING_CODE)
 * 	TEST aggiungere mapping MediaControl => resistance value
 * 	to send string messages to radio use ids: on adroid app store a json file mapping id to message
 */

MediaControlCarduinoNode::MediaControlCarduinoNode(uint8_t id, uint8_t cs, uint8_t interruptPin, uint8_t encoderClk, uint8_t encoderDt, uint8_t encoderSw, uint8_t digiPotCs, uint8_t digiPotUd, uint8_t digiPotInc, uint8_t buzzer, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, false, true) {
    this->restoreSettings();

	this->canExecutor->addExecutor(new MediaControlCanEvent());

	pcf8574 = new PCF8574(0x20, NODE_SDA, NODE_SCL);

	pcf8574->pinMode(encoderClk, INPUT);
	pcf8574->pinMode(encoderDt, INPUT);
	pcf8574->pinMode(encoderSw, INPUT);
	pcf8574->pinMode(digiPotCs, OUTPUT);
	pcf8574->pinMode(digiPotUd, OUTPUT);
	pcf8574->pinMode(digiPotInc, OUTPUT);

	bool i2cValid = pcf8574->begin();
	printlnWrapper("MediaControlCarduinoNode::MediaControlCarduinoNode() i2cValid " + String(i2cValid ? "true" : "false") + " " + String(millis()));
	
	versatileEncoder = new Versatile_RotaryEncoder(encoderClk, encoderDt, encoderSw, pcf8574);
	this->lastRead = 0;
	if(i2cValid) {
		versatileEncoder->setHandleRotate([this](uint8_t rotation){
			printlnWrapper("MediaControlCarduinoNode::MediaControlCarduinoNode() versatileEncoder->setHandleRotate rotation " + String(rotation) + " " + String(millis()));
			if(!this->canRead()) {
				return;
			}
			this->lastRead = millis();
			if(rotation == 255) { // clockwise
				// this->sendMediaControlMessage(&MediaControl::VOLUME_UP);
				// Serial.println("VOLUME_UP");
				this->pressButton(MediaControl::VOLUME_UP.resistance);
			} else if (rotation == 1) { //counter clockwise
				// this->sendMediaControlMessage(&MediaControl::VOLUME_DOWN);
				// Serial.println("VOLUME_DOWN");
				this->pressButton(MediaControl::VOLUME_DOWN.resistance);
			}
		});
		versatileEncoder->setHandlePressRelease([this](){
			printlnWrapper("MediaControlCarduinoNode::MediaControlCarduinoNode() versatileEncoder->setHandlePressRelease " + String(millis()));
			if(!this->canRead()) {
				return;
			}
			this->lastRead = millis();
			// this->sendMediaControlMessage(&MediaControl::PLAY_PAUSE);
			this->pressButton(MediaControl::PLAY_PAUSE.resistance);
		});
		versatileEncoder->setHandleDoublePressRelease([this](){
			printlnWrapper("MediaControlCarduinoNode::MediaControlCarduinoNode() versatileEncoder->setHandleDoublePressRelease " + String(millis()));
			if(!this->canRead()) {
				return;
			}
			this->lastRead = millis();
			// this->sendMediaControlMessage(&MediaControl::NEXT);
			this->pressButton(MediaControl::NEXT.resistance);
		});
		versatileEncoder->setHandleLongPress([this](){
			printlnWrapper("MediaControlCarduinoNode::MediaControlCarduinoNode() versatileEncoder->setHandleLongPress " + String(millis()));
			if(!this->canRead()) {
				return;
			}
			this->lastRead = millis();
			if(this->readyToStartSwcPairingFlag) {
				this->readyToStartSwcPairingFlag = false;
				if(this->resetReadyToPairFlagTask != nullptr && this->resetReadyToPairFlagTask->isEnabled()) {
					this->scheduler->deleteTask(*this->resetReadyToPairFlagTask);
				}
				this->playTone(&Event::WARNING_SEVERITY_MEDIUM);
				this->startSwcPairing();
			} else {
				this->sendMediaControlMessage(&MediaControl::LONG_PRESS);
			}
		});
	}

	x9c103s = new X9C103S(digiPotInc, digiPotUd, digiPotCs, pcf8574);
	x9c103s->initializePot();

	writeResistance(0); // Set initial resistance to 0

	this->buzzerPin = buzzer;
	this->readyToStartSwcPairingFlag = false;
};

void MediaControlCarduinoNode::startSwcPairing() {
	printlnWrapper("MediaControlCarduinoNode::startSwcPairing");
	auto swcPairingCallback = [&](auto&& self, uint8_t mediaControlIndex) -> void {
		if(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->id == MediaControl::LONG_PRESS.id) {
			mediaControlIndex++; // Skip LONG_PRESS control
		}

		if (mediaControlIndex == MEDIA_CONTROL_SIZE) {
			this->printlnWrapper("MediaControlCarduinoNode::startSwcPairing finished " + String(millis()));
			this->playTone(&Event::WARNING_SEVERITY_MEDIUM);
			this->pairing = false;
			return; // Finished pairing all controls
		}

		this->playTone(&Event::WARNING_SEVERITY_LOW);

		this->writeResistance(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->resistance);
		this->printlnWrapper("MediaControlCarduinoNode::startSwcPairing start pressing " + String(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->name) + " " + String(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->resistance));

		this->delayTask(SWC_PAIRING_INTERVAL, [&, mediaControlIndex, self]() mutable {
			this->writeResistance(0);
			this->printlnWrapper("MediaControlCarduinoNode::startSwcPairing Stop pressing " + String(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->name) + " " + String(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->resistance));

			this->delayTask(SWC_WAITING_PAIRING_INTERVAL, [&, mediaControlIndex, self]() mutable {
				// this->printlnWrapper("stop waiting PIN " + String(mediaControlIndex + 1) + " " + String(millis()));
				self(self, mediaControlIndex + 1);
			});
		});
	};

	// Avvio iniziale
	this->delayTask(SWC_FIRST_WAITING_PAIRING_INTERVAL, [&, swcPairingCallback]() {
		swcPairingCallback(swcPairingCallback, 0);
	});

	this->pairing = true;
}

void MediaControlCarduinoNode::readyToStartSwcPairing() {
	printlnWrapper("MediaControlCarduinoNode::readyToStartSwcPairing");
	this->readyToStartSwcPairingFlag = true;
	playTone(&Event::WARNING_SEVERITY_LOW);
	this->resetReadyToPairFlagTask = this->delayTask(SWC_FLAG_READY_TO_PAIR_RESET_INTERVAL, [&](){
		if(this->readyToStartSwcPairingFlag) {
			printlnWrapper("MediaControlCarduinoNode::readyToStartSwcPairing reset readyToStartSwcPairingFlag");
			this->playTone(&Event::WARNING_SEVERITY_MEDIUM);
			this->readyToStartSwcPairingFlag = false;
		}
	});
	//TODO: send message to radio saying long press to start swc pairing
}

void MediaControlCarduinoNode::pressButton(uint8_t resistance) {
	if(this->pairing) {
		return; // Do not press button if pairing is in progress
	}

	this->writeResistance(resistance);

	if(releaseButtonTask != nullptr && releaseButtonTask->isEnabled()) {
		// releaseButtonTask->disable();
		this->scheduler->deleteTask(*releaseButtonTask);
	}
	releaseButtonTask = this->delayTask(SWC_PRESS_INTERVAL, [&](){
		this->writeResistance(0); // Release button
		this->printlnWrapper("MediaControlCarduinoNode::pressButton released button " + String(millis()));
	});
}

void MediaControlCarduinoNode::writeResistance(uint8_t resistance) {
	this->printlnWrapper("MediaControlCarduinoNode::writeResistance " + String(resistance) + " " + String(millis()));
	this->x9c103s->setResistance(resistance);
};

void MediaControlCarduinoNode::sendMediaControlMessage(const MediaControl *mediaControl) {
	MediaControlMessage *m = new MediaControlMessage(mediaControl);
	this->sendCanbusMessage(m);
	delete m;
};

void MediaControlCarduinoNode::loop() {
	CarduinoNode::loop();
	versatileEncoder->ReadEncoder();
};

bool MediaControlCarduinoNode::canRead() {
	return millis() > this->lastRead + ENCODER_READING_INTERVAL;
};

//TODO: create enum to be passed to playTone function (in place of Event)
void MediaControlCarduinoNode::playTone(const Event *toneEvent) {
	if(toneEvent->id == Event::WARNING_SEVERITY_LOW.id) {
		printlnWrapper("MediaControlCarduinoNode::playTone LOW severity tone");
		this->startTone(440, 200); // A4 per 200ms
	} else if(toneEvent->id == Event::WARNING_SEVERITY_MEDIUM.id) {
		printlnWrapper("MediaControlCarduinoNode::playTone MEDIUM severity tone");
		auto playToneRecursive = [&](auto&& self, uint8_t toneIndex) -> void {
			if (toneIndex == 2) return; // Limit to 2 tones for medium severity

			// printlnWrapper("MediaControlCarduinoNode::playTone play next tone MEDIUM " + String(toneIndex));
			this->startTone(660, 150); // E5

			this->delayTask(150 + 200, [&, toneIndex, self]() mutable {
				// printlnWrapper("MediaControlCarduinoNode::playTone CALL play next tone MEDIUM");
				self(self, toneIndex + 1);
			});
		};

		// Avvio iniziale
		playToneRecursive(playToneRecursive, 0);
	} else if(toneEvent->id == Event::WARNING_SEVERITY_HIGH.id) {
		printlnWrapper("MediaControlCarduinoNode::playTone HIGH severity tone");
		auto playToneRecursive = [&](auto&& self, uint8_t toneIndex) -> void {
			if (toneIndex == 6) return; // Limit to 6 tones for high severity

			// printlnWrapper("MediaControlCarduinoNode::playTone play next tone HIGH " + String(toneIndex));
			this->startTone(880, 100); // A5

			this->delayTask(100 + 75, [&, toneIndex, self]() mutable {
				// printlnWrapper("MediaControlCarduinoNode::playTone CALL play next tone HIGH");
				self(self, toneIndex + 1);
			});
		};

		// Avvio iniziale
		playToneRecursive(playToneRecursive, 0);
	}
}

void MediaControlCarduinoNode::startTone(int freq, int duration) {
	// printlnWrapper("MediaControlCarduinoNode::startTone " + String(freq) + " " + String(duration));
	analogWriteFreq(freq);
	analogWrite(buzzerPin, 512); // duty cycle 50%
	this->delayTask(duration, [&](){
		// printlnWrapper("MediaControlCarduinoNode::startTone turn off buzzer");
		analogWrite(buzzerPin, 0);   // stop tone
	});
}
