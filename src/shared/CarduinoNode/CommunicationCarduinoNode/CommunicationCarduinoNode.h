#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"

#define ENCODER_READING_INTERVAL 75

class CommunicationCarduinoNode : public CarduinoNode {
    public:
        CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();

    private:
};
