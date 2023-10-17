#pragma once

#define USE_STD_FUNCTION

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"

class MediaControlCarduinoNode : public CarduinoNode {
    public:
        MediaControlCarduinoNode(uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();
        void sendMediaControlMessage(const MediaControl*);

    private:
        Versatile_RotaryEncoder *versatileEncoder;
};
