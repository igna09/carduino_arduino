#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/CarduinoNode/KlineCarduinoNode/KlineCarduinoNode.h"

KlineCarduinoNode *klineCarduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(115200);

  klineCarduinoNode = new KlineCarduinoNode(D2, D1, D3, D0, "SSID_MAIN_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();

void loop(void) {
	printFreeHeap("loop");
  	klineCarduinoNode->loop();
	printFreeHeap("loop");
	// if(millis() > lastSent + 2000) {
	// 	lastSent = millis();
	// 	float v = 45.7;
	// 	printFreeHeap("loop");
	// 	CarstatusMessage m(&Carstatus::ENGINE_OIL_TEMPERATURE, v);
	// 	klineCarduinoNode->sendCanbusMessage(&m);
	// 	printFreeHeap("loop");
	// }
}
