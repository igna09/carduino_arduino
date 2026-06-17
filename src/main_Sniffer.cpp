#include <Arduino.h>

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "./nodes/Sniffer/Sniffer.h"

Sniffer *sniffer;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(BAUD_RATE);
  randomSeed(analogRead(0));

  sniffer = new Sniffer(0xFF, 8, 2, "SSID_SNIFFER_CARDUINO_NODE", "pwd12345");
}

unsigned long lastSent = 0;

void loop(void) {
  sniffer->loop();
  // if(millis() > lastSent + 10000 && lastSent == 0) {
      // Serial.println(micros() - lastSent);
			// lastSent = micros();
      // uint8_t a[] = {0x01};
			// CanbusMessage *message = new CanbusMessage(0b10000000010, a, 1);
			// carduinoNode->manageReceivedCanbusMessage(message);
  //     carduinoNode->printlnWrapper("test");
  // carduinoNode->handleReceivedSerialMessage("GET_SETTINGS;;;");
  //   carduinoNode->sendEvent(&Event::DISABLE, ALL_NODES);
	// }
}
