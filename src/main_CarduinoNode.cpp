#include <Arduino.h>

#include "./shared/CarduinoNode/CarduinoNode.h"

CarduinoNode *carduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(115200);

  carduinoNode = new CarduinoNode(0,0,"SSID_CARDUINO_NODE","pwd12345");
}

void loop(void) {
  carduinoNode->loop();
}
