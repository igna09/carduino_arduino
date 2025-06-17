#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include "X9C103S.h"

#define SWC_PRESS_INTERVAL  65
#define SWC_PAIRING_INTERVAL  5000
#define SWC_WAITING_PAIRING_INTERVAL  1000
#define SWC_FIRST_WAITING_PAIRING_INTERVAL  5000
#define SWC_PIN_SIZE 8

// #define ENCODER_READING_INTERVAL 75
#define ENCODER_READING_INTERVAL 0

class MediaControlCarduinoNode : public CarduinoNode {
    public:
        MediaControlCarduinoNode(uint8_t id, uint8_t clk, uint8_t dt, uint8_t sw, int cs, int interruptPin, uint8_t digiPotCs, uint8_t digiPotUd, uint8_t digiPotInc, const char *ssid, const char *password);
        void loop();
        void sendMediaControlMessage(const MediaControl*);
        void releaseButtonDelayed();

    private:
        Versatile_RotaryEncoder *versatileEncoder;
        X9C103S *x9c103s;
        Task *noButtonTask;
        unsigned long lastRead;
        bool canRead();
};
