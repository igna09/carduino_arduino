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

  carduinoNode = new MainCarduinoNode(D8,D0,"SSID_MAIN_CARDUINO_NODE","pwd12345");
}

void loop(void) {
  carduinoNode->loop();
}
