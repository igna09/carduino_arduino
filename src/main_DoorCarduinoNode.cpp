#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "shared/CarduinoNode/DoorCarduinoNode/DoorCarduinoNode.h"

DoorCarduinoNode *doorCarduinoNode;

void setup(void) {
  	Serial.begin(BAUD_RATE);
//   randomSeed(analogRead(0));

  	doorCarduinoNode = new DoorCarduinoNode(0x03, D8, D0, "SSID_DOOR_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();

void loop(void) {
  	doorCarduinoNode->loop();
	// if(millis() > lastSent + 1000) {
	// 		lastSent = millis();
	// 		printFreeHeap("loop");
	// 		const MediaControl *mc = (const MediaControl*) MediaControl::getValueById(random(3));
	// 		Serial.println(mc->name);
	// 		mediaControlCarduinoNode->sendMediaControlMessage(mc);
	// }
}
