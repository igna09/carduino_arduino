#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "./shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"

MainCarduinoNode *carduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(BAUD_RATE);
  randomSeed(analogRead(0));

  carduinoNode = new MainCarduinoNode(0x00, D8, D0, "SSID_MAIN_CARDUINO_NODE", "pwd12345");
}

// unsigned long lastSent = millis();

void loop(void) {
  carduinoNode->loop();
  // if(millis() > lastSent + 1000) {
	// 		lastSent = millis();
      // uint8_t a[] = {0x01};
			// CanbusMessage *message = new CanbusMessage(0b10000000010, a, 1);
			// carduinoNode->manageReceivedCanbusMessage(message);
  //     carduinoNode->printlnWrapper("test");
	// }
}
