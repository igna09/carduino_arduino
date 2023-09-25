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
  randomSeed(analogRead(0));

  klineCarduinoNode = new KlineCarduinoNode(0,0,"SSID_MAIN_CARDUINO_NODE","pwd12345");
}

void loop(void) {
  klineCarduinoNode->loop();
}
