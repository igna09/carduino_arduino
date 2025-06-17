#include "MediaControlCarduinoNode.h"

/**
 * TODO: 
 * 	taskscheduler che riporta la resistenza a 0 dopo tot di ms (da recuperare da main node)
 * 	portare la gestione del mapping dei pulsanti da main node a qui
 * 	durante la registrazione dei pulsanti mandare messaggio alla radio (non visibile perché siamo su app radio swc)
 * 	aggiungere buzzer
 * 	far suonare buzzer durante il cambio pulsanti registrazione
 * 	aggiungere mapping MediaControl => resistance value
 * 	far partire registrazione dei pulsanti solo quando viene premuto il pulsante
 */

MediaControlCarduinoNode::MediaControlCarduinoNode(uint8_t id, uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, uint8_t digiPotCs, uint8_t digiPotUd, uint8_t digiPotInc, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, true, true) {
    this->restoreSettings();
	
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
			this->x9c103s->setResistance(100);
			this->releaseButtonDelayed();
		} else if (rotation == 1) { //counter clockwise
			// this->sendMediaControlMessage(&MediaControl::VOLUME_DOWN);
			// Serial.println("VOLUME_DOWN");
			this->x9c103s->setResistance(80);
			this->releaseButtonDelayed();
		}
	});
	versatileEncoder->setHandlePressRelease([this](){
		if(!this->canRead()) {
			return;
		}
		this->lastRead = millis();
		// this->sendMediaControlMessage(&MediaControl::PLAY_PAUSE);
		this->x9c103s->setResistance(60);
		this->releaseButtonDelayed();
	});
	versatileEncoder->setHandleDoublePressRelease([this](){
		if(!this->canRead()) {
			return;
		}
		this->lastRead = millis();
		// this->sendMediaControlMessage(&MediaControl::NEXT);
		this->x9c103s->setResistance(40);
		this->releaseButtonDelayed();
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
};

void MediaControlCarduinoNode::releaseButtonDelayed() {
	if(noButtonTask->isEnabled()) {
		noButtonTask->cancel();
	}

	noButtonTask = this->delayTask(SWC_PRESS_INTERVAL, [&](){
		this->x9c103s->setResistance(0);
	});
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
