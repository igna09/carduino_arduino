#pragma once

#include <Arduino.h>
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "executors/AllCanEvent/AllCanEvent.h"
#include "shared/SharedDefinitions.h"
#include "shared/enums/Event.h"

class Sniffer : public CarduinoNode {
    public:
        Sniffer(uint8_t id, uint8_t cs, uint8_t interruptPin, const char *ssid, const char *password);
        void loop();

    private:
};
