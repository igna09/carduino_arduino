#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/CarduinoNode/MainCarduinoNode.h"

MainCarduinoNode carduinoNode(0,0,"ssid","pwd");

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {
  carduinoNode.loop();
}
