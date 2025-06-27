#pragma once

#include <Arduino.h>
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include "shared/enums/Event.h"
#include "X9C103S.h"
#include "nodes/MediaControlCarduinoNode/executors/MediaControlCanEvent/MediaControlCanEvent.h"

#define SWC_PRESS_INTERVAL  65
#define SWC_PAIRING_INTERVAL  5000
#define SWC_WAITING_PAIRING_INTERVAL  1000
#define SWC_FIRST_WAITING_PAIRING_INTERVAL  5000
#define SWC_PIN_SIZE 8
#define SWC_FLAG_READY_TO_PAIR_RESET_INTERVAL 30000

// #define ENCODER_READING_INTERVAL 75
#define ENCODER_READING_INTERVAL 0
// #define ENCODER_READING_INTERVAL SWC_PRESS_INTERVAL

class MediaControlCarduinoNode : public CarduinoNode {
    public:
        MediaControlCarduinoNode(uint8_t id, uint8_t cs, uint8_t interruptPin, uint8_t encoderClk, uint8_t encoderDt, uint8_t encoderSw, uint8_t digiPotCs, uint8_t digiPotUd, uint8_t digiPotInc, uint8_t buzzer, const char *ssid, const char *password);
        void loop();
        void sendMediaControlMessage(const MediaControl*);
        void pressButton(uint8_t resistance);//convert to MediaControl
        void startSwcPairing();
        void playTone(const Event*);
        void startTone(int freq, int duration);
        void readyToStartSwcPairing();

    private:
        Versatile_RotaryEncoder *versatileEncoder;
        X9C103S *x9c103s;
        Task *releaseButtonTask;
        unsigned long lastRead;
        uint8_t buzzerPin;
        bool canRead();
        bool readyToStartSwcPairingFlag;
        Task *resetReadyToPairFlagTask;
        PCF8574 *pcf8574;
        bool pairing;
        void writeResistance(uint8_t resistance);
};
