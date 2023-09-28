#pragma once

#include <Arduino.h>
#include "../CarduinoNode/CarduinoNode.h"
#include <SoftwareSerial.h>
#include "KLineKWP1281Lib.h"
// #include "configuration.h"
// #include "communication.h"
#include "KlineEcuEnum.h"
#include "EcuToRead.h"

class KlineCarduinoNode : CarduinoNode {
    public:
        KlineCarduinoNode(uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, char *ssid,  char *password);

        void loop();

    private:
        KLineKWP1281Lib *kLine;
		SoftwareSerial *softwareSerial;
		uint8_t pin_rx;
    	uint8_t pin_tx;
		EcusToRead ecusToRead;
};
