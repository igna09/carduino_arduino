#include "MediaControlCarduinoNode.h"

/**
 * TODO: 
 * 	TEST taskscheduler che riporta la resistenza a 0 dopo tot di ms (da recuperare da main node)
 * 	TEST portare la registrazione dei pulsanti da main node a qui
 * 	durante la registrazione dei pulsanti mandare messaggio alla radio (non visibile perché siamo su app radio swc, BLE_PAIRING_CODE)
 * 	TEST aggiungere buzzer
 * 	TEST far suonare buzzer durante il cambio pulsanti registrazione
 * 	TEST aggiungere mapping MediaControl => resistance value
 * 	far partire registrazione dei pulsanti solo quando viene premuto il pulsante
 */

MediaControlCarduinoNode::MediaControlCarduinoNode(uint8_t id, uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, uint8_t digiPotCs, uint8_t digiPotUd, uint8_t digiPotInc, uint8_t buzzer, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, true, true) {
    this->restoreSettings();

	this->canExecutor->addExecutor(new MediaControlCanEvent());
	
	versatileEncoder = new Versatile_RotaryEncoder(clk, dt, sw);
	this->lastRead = 0;
	versatileEncoder->setHandleRotate([this](uint8_t rotation){
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
		if(!this->canRead()) {
			return;
		}
		this->lastRead = millis();
		// this->sendMediaControlMessage(&MediaControl::PLAY_PAUSE);
		this->pressButton(MediaControl::PLAY_PAUSE.resistance);
	});
	versatileEncoder->setHandleDoublePressRelease([this](){
		if(!this->canRead()) {
			return;
		}
		this->lastRead = millis();
		// this->sendMediaControlMessage(&MediaControl::NEXT);
		this->pressButton(MediaControl::NEXT.resistance);
	});
	versatileEncoder->setHandleLongPress([this](){
		if(!this->canRead()) {
			return;
		}
		this->lastRead = millis();
		this->sendMediaControlMessage(&MediaControl::LONG_PRESS);
	});

	x9c103s = new X9C103S(digiPotInc, digiPotUd, digiPotInc);
	x9c103s->initializePot();

	this->buzzerPin = buzzer;
};

void MediaControlCarduinoNode::startSwcPairing() {
	std::function<void(uint8_t)> swcPairingCallback = [&](uint8_t mediaControlIndex){
		this->playTone(&Event::WARNING_SEVERITY_LOW);

		this->x9c103s->setResistance(((MediaControl*) MediaControl::getValues()[mediaControlIndex])->resistance);
		this->printlnWrapper("start pressing PIN " + String(mediaControlIndex) + " " + String(millis()));
		this->delayTask(SWC_PAIRING_INTERVAL, [&](){
			this->x9c103s->setResistance(0);
            this->printlnWrapper("Stop pressing PIN " + String(mediaControlIndex) + " " + String(millis()));
			if(mediaControlIndex < (MEDIA_CONTROL_SIZE - 1)) {
                this->printlnWrapper("Start waiting PIN " + String(mediaControlIndex + 1) + " " + String(millis()));
				this->delayTask(SWC_WAITING_PAIRING_INTERVAL, [&](){
            		this->printlnWrapper("stop waiting PIN " + String(mediaControlIndex + 1) + " " + String(millis()));
					swcPairingCallback(mediaControlIndex + 1);
				});
			} else {
				this->playTone(&Event::WARNING_SEVERITY_MEDIUM);
			}
		});
	};
	this->delayTask(SWC_FIRST_WAITING_PAIRING_INTERVAL, [&](){
		swcPairingCallback(0);
	});
}

void MediaControlCarduinoNode::pressButton(uint8_t resistance) {
	this->x9c103s->setResistance(resistance);

	if(releaseButtonTask->isEnabled()) {
		releaseButtonTask->cancel();
	}

	releaseButtonTask = this->delayTask(SWC_PRESS_INTERVAL, [&](){
		this->x9c103s->setResistance(0);
	});
}

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

void MediaControlCarduinoNode::playTone(const Event *toneEvent) {
	if(toneEvent->id == Event::WARNING_SEVERITY_LOW.id) {
		this->startTone(440, 200); // A4 per 200ms
	} else if(toneEvent->id == Event::WARNING_SEVERITY_MEDIUM.id) {
		std::function<void(uint8_t)> playToneRecursive = [&](uint8_t toneIndex){
			this->startTone(660, 150); // E5
			if(toneIndex < 2) {
				this->delayTask(150 + 200, [&](){ // 150 time to wait previous tone to end, 200 delay between tones
					playToneRecursive(++toneIndex);
				});
			}
		};
		playToneRecursive(0);
	} else if(toneEvent->id == Event::WARNING_SEVERITY_HIGH.id) {
		std::function<void(uint8_t)> playToneRecursive = [&](uint8_t toneIndex){
			this->startTone(880, 100); // A5
			if(toneIndex < 6) {
				this->delayTask(100 + 150, [&](){ // 100 time to wait previous tone to end, 150 delay between tones
					playToneRecursive(++toneIndex);
				});
			}
		};
		playToneRecursive(0);
	}
}

void MediaControlCarduinoNode::startTone(int freq, int duration) {
  analogWriteFreq(freq);
  analogWrite(buzzerPin, 1024); // duty cycle 100%
  this->delayTask(duration, [&](){
  	analogWrite(buzzerPin, 0);   // stop tone
  });
}
