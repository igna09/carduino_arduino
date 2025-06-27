#include <Arduino.h>

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "nodes/DoorCarduinoNode/DoorCarduinoNode.h"

DoorCarduinoNode *doorCarduinoNode;

void setup(void) {
  	Serial.begin(BAUD_RATE);
//   randomSeed(analogRead(0));

  	doorCarduinoNode = new DoorCarduinoNode(0x03, D8, D2, "SSID_DOOR_CARDUINO_NODE", "pwd12345");
}

unsigned long lastSent = 0;

void loop(void) {
  	doorCarduinoNode->loop();
	// if(millis() > lastSent + 15000 && lastSent == 0) {
	// 	lastSent = millis();
		// Serial.println("first loop after 15 seconds");
		// doorCarduinoNode->saveSettings();
		// printFreeHeap("loop");
		// const MediaControl *mc = (const MediaControl*) MediaControl::getValueById(random(3));
		// Serial.println(mc->name);
		// mediaControlCarduinoNode->sendMediaControlMessage(mc);
	// }
}
