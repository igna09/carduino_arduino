#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/CarduinoNode/MainCarduinoNode.h"

MainCarduinoNode *carduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(115200);
  randomSeed(analogRead(0));

  carduinoNode = new MainCarduinoNode(0,0,"SSID_MAIN_CARDUINO_NODE","pwd12345");
}

void loop(void) {
  carduinoNode->loop();
}
