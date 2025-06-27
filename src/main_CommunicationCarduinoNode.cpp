#include <Arduino.h>

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "nodes/CommunicationCarduinoNode/CommunicationCarduinoNode.h"

CommunicationCarduinoNode *communicationCarduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  	Serial.begin(BAUD_RATE);
//   randomSeed(analogRead(0));

	// TODO: update encoder pins
  	communicationCarduinoNode = new CommunicationCarduinoNode(0x04, 32, 33, "SSID_COMMUNICATION_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();

void loop(void) {
  	communicationCarduinoNode->loop();
	// if(millis() > lastSent + 1000) {
	// 		lastSent = millis();
	// 		printFreeHeap("loop");
	// 		const MediaControl *mc = (const MediaControl*) MediaControl::getValueById(random(3));
	// 		Serial.println(mc->name);
	// 		mediaControlCarduinoNode->sendMediaControlMessage(mc);
	// }
}
