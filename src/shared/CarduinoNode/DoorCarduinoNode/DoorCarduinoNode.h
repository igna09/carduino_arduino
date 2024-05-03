#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"

class DoorCarduinoNode : public CarduinoNode {
    public:
        DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();
};
