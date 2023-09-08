#include <Arduino.h>
#include <TaskScheduler.h>

#include "./shared/CarduinoNode/MainCarduinoNode.h"

Scheduler runner;
MainCarduinoNode carduinoNode(0,0,"ssid","pwd",&runner);

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  Serial.begin(115200);
}

void loop(void) {

  //delay(5);
}
