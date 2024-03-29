#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "./shared/CarduinoNode/KlineCarduinoNode/KlineCarduinoNode.h"

KlineCarduinoNode *klineCarduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(BAUD_RATE);

  klineCarduinoNode = new KlineCarduinoNode(0x01, D2, D1, D3, D0, "SSID_KLINE_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();
// bool sent = false;

void loop(void) {
  	klineCarduinoNode->loop();
	// if(millis() > 5000 & !sent) {
	// 	sent = true;
	// 	klineCarduinoNode->otaStartup();
	// }
	// if(millis() > lastSent + 2000) {
		// 	lastSent = millis();
		// 	float v = 45.7;
		// 	printFreeHeap("loop");
		// 	CarstatusMessage m(&Carstatus::ENGINE_OIL_TEMPERATURE, v);
		// 	klineCarduinoNode->sendCanbusMessage(&m);
		// 	printFreeHeap("loop");
	// }
}
