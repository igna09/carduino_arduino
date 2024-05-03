#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeWriteSetting/DoorNodeWriteSetting.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeGetSettings/DoorNodeGetSettings.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"

class DoorCarduinoNode : public CarduinoNode {
    public:
        DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);

        bool lowerMirrorsOnReverse;

        void loop();
        void setup();
        void setdown();
};
