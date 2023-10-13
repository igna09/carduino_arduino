#include <Arduino.h>
#include "shared/CarduinoNode/KlineCarduinoNode/ValueToReadEnum.h"
#include "shared/utils.h"

void setup()
{
  Serial.begin(115200);
}

void loop()
{
  printFreeHeap("test::loop");

  uint8_t ecusToReadSize = ValueToReadEnum::getEcusToReadSize();
  Serial.println(ecusToReadSize);

  KlineEcuEnum **ecusToRead = ValueToReadEnum::getEcusToRead();
  for(uint8_t i = 0; i < ecusToReadSize; i++) {
    Serial.println(ecusToRead[i]->name);
  }

  delete ecusToRead;

  delay(1000);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
