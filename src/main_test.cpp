#include <Arduino.h>
#include "Versatile_RotaryEncoder.h"

Versatile_RotaryEncoder *versatileEncoder;

void setup()
{
  Serial.begin(115200);

  versatileEncoder = new Versatile_RotaryEncoder(D1, D2, RX);

  versatileEncoder->setHandleRotate([](uint8_t rotation){
		if(rotation == 255) { // clockwise
			Serial.println("vol up");
		} else if (rotation == 1) { //counter clockwise
			Serial.println("vol down");
		}
	});
	versatileEncoder->setHandlePressRelease([](){
			Serial.println("play pause");
	});
}

void loop()
{
  versatileEncoder->ReadEncoder();
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
