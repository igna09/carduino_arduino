#include <Arduino.h>
#include <TaskScheduler.h>

#include "shared/CarduinoNode/MediaControlCarduinoNode/MediaControlCarduinoNode.h"

MediaControlCarduinoNode *mediaControlCarduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  	Serial.begin(115200);
//   randomSeed(analogRead(0));

	// TODO: update encoder pins
  	mediaControlCarduinoNode = new MediaControlCarduinoNode(0,0,0, D3, D0, "SSID_MEDIA_CONTROL_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();

void loop(void) {
  	mediaControlCarduinoNode->loop();
	// if(millis() > lastSent + 1000) {
	// 		lastSent = millis();
	// 		printFreeHeap("loop");
	// 		const MediaControl *mc = (const MediaControl*) MediaControl::getValueById(random(3));
	// 		Serial.println(mc->name);
	// 		mediaControlCarduinoNode->sendMediaControlMessage(mc);
	// }
}
