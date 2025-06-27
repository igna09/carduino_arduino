#include <Arduino.h>

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable
#include <TaskScheduler.h>

#include "./shared/SharedDefinitions.h"
#include "CarduinoNode/MediaControlCarduinoNode/MediaControlCarduinoNode.h"

#define DIGIPOT_CS 0
#define DIGIPOT_UD 1
#define DIGIPOT_INC 2

#define ENCODER_CLK 3
#define ENCODER_DT 4
#define ENCODER_SW 5

#define BUZZER D0

MediaControlCarduinoNode *mediaControlCarduinoNode;

/**
 * convenzione:
 * interi 4 bytes
 * float 4 bytes (parte intera) + 1 byte (parte decimale)
*/

void setup(void) {
  	Serial.begin(BAUD_RATE);
//   randomSeed(analogRead(0));

	// TODO: update encoder pins
  	mediaControlCarduinoNode = new MediaControlCarduinoNode(
		0x02,
		D8, 
		D2, 
		ENCODER_CLK, 
		ENCODER_DT, 
		ENCODER_SW, 
		DIGIPOT_CS, 
		DIGIPOT_UD, 
		DIGIPOT_INC, 
		BUZZER,
		"SSID_MEDIA_CONTROL_CARDUINO_NODE",
		"pwd12345"
	);
}

// unsigned long lastSent = millis();
bool sent = false;

void loop(void) {
  	mediaControlCarduinoNode->loop();
	// if(millis() > 1000 && !sent) {
	// 	sent = true;
	// 	mediaControlCarduinoNode->playTone(&Event::WARNING_SEVERITY_HIGH);
	// }
//   byte error, address;
//   int nDevices;
 
//   Serial.println("Scanning...");
 
//   nDevices = 0;
//   for(address = 1; address < 127; address++ ) 
//   {
//     // The i2c_scanner uses the return value of
//     // the Write.endTransmisstion to see if
//     // a device did acknowledge to the address.
//     Wire.beginTransmission(address);
//     error = Wire.endTransmission();
 
//     if (error == 0)
//     {
//       Serial.print("I2C device found at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.print(address,HEX);
//       Serial.println("  !");
 
//       nDevices++;
//     }
//     else if (error==4) 
//     {
//       Serial.print("Unknow error at address 0x");
//       if (address<16) 
//         Serial.print("0");
//       Serial.println(address,HEX);
//     }    
//   }
//   if (nDevices == 0)
//     Serial.println("No I2C devices found\n");
//   else
//     Serial.println("done\n");
	// if(millis() > lastSent + 1000) {
	// 		lastSent = millis();
	// 		printFreeHeap("loop");
	// 		const MediaControl *mc = (const MediaControl*) MediaControl::getValueById(random(3));
	// 		Serial.println(mc->name);
	// 		mediaControlCarduinoNode->sendMediaControlMessage(mc);
	// }
}
