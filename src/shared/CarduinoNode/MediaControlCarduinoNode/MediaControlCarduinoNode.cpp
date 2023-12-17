#include "MediaControlCarduinoNode.h"

MediaControlCarduinoNode::MediaControlCarduinoNode(uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(cs, interruptPin, ssid,  password) {
    versatileEncoder = new Versatile_RotaryEncoder(clk, dt, sw);

	versatileEncoder->setHandleRotate([this](uint8_t rotation){
		if(rotation == 255) { // clockwise
			this->sendMediaControlMessage(&MediaControl::VOLUME_UP);
			// Serial.println("VOLUME_UP");
		} else if (rotation == 1) { //counter clockwise
			this->sendMediaControlMessage(&MediaControl::VOLUME_DOWN);
			// Serial.println("VOLUME_DOWN");
		}
	});
	versatileEncoder->setHandlePressRelease([this](){
		this->sendMediaControlMessage(&MediaControl::PLAY_PAUSE);
		// Serial.println("PLAY_PAUSE");
	});
	versatileEncoder->setHandleLongPressRelease([this](){
		this->sendMediaControlMessage(&MediaControl::LONG_PRESS);
	});
	versatileEncoder->setHandleDoublePressRelease([this](){
		// Serial.println("NEXT");
		this->sendMediaControlMessage(&MediaControl::NEXT);
	});
};

void MediaControlCarduinoNode::sendMediaControlMessage(const MediaControl *mediaControl) {
	MediaControlMessage *m = new MediaControlMessage(mediaControl);
	this->sendCanbusMessage(m);
	delete m;
};

void MediaControlCarduinoNode::loop () {
	CarduinoNode::loop();
	versatileEncoder->ReadEncoder();
};
