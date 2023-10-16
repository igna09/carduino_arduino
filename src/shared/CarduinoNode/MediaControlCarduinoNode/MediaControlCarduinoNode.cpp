#include "MediaControlCarduinoNode.h"

MediaControlCarduinoNode::MediaControlCarduinoNode(uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(cs, interruptPin, ssid,  password) {
    versatileEncoder = new Versatile_RotaryEncoder(clk, dt, sw);

	versatileEncoder->setHandleRotate([this](uint8_t rotation){
		if(rotation > 0) { // clockwise
			this->sendMediaControlMessage(&MediaControl::VOLUME_UP);
		} else { //counter clockwise
			this->sendMediaControlMessage(&MediaControl::VOLUME_DOWN);
		}
	});
	versatileEncoder->setHandlePressRelease([this](){
		this->sendMediaControlMessage(&MediaControl::PLAY_PAUSE);
	});
};

void MediaControlCarduinoNode::sendMediaControlMessage(const MediaControl *mediaControl) {
	MediaControlMessage m(mediaControl);
	this->sendCanbusMessage(&m);
};

void MediaControlCarduinoNode::loop () {
	CarduinoNode::loop();
	versatileEncoder->ReadEncoder();
};
