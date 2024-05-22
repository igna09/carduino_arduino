#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeWriteSetting/DoorNodeWriteSetting.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeGetSettings/DoorNodeGetSettings.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeEvent/DoorNodeEvent.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeCarstatus/DoorNodeCarstatus.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include <PCF8574.h>
#include "shared/CarduinoNode/DoorCarduinoNode/callbacks/StopMovingMirrorsCallback.h"
#include "shared/CarduinoNode/DoorCarduinoNode/callbacks/BatteryVoltageCallback.h"

#define PIN_MIRROR_A P0
#define PIN_MIRROR_B P1
#define PIN_MIRROR_ENABLE P2
#define PIN_CLOSED_MIRRORS_RELAY P3 // CHECK IF MIRROR IS CLOSED (MIRROR RELAY)
#define PIN_OPEN_MIRRORS P4 // PIN TO CONTROL OPENING/CLOSING MIRRORS
#define PIN_MIRROR_SELECTOR_ON_CLOSED P5 // PIN TO CHECK IF SELECTOR IS ON CLOSED
#define MIRRORS_MOVING_TIME 500

#define VOLTAGE_READING_INTERVAL 1000
#define VOLTAGE_READING_PIN A0
#define VOLTAGE_READING_PIN_RESISTOR 220000 //220K OHM

class DoorCarduinoNode : public CarduinoNode {
    public:
        DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);

        PCF8574 *pcf8574;
        bool lowerMirrorsOnReverse;
        // bool disabledKeyboard;
        bool closedMirrors;
        bool foldingMirrors;
        bool movingMirrors;
        Task *stopMoveMirrorsTask;
        Event *lastReceivedEvent;
        bool reverse;
        bool closedSelector;

        void loop();
        void setup();
        void setdown();
        void closeMirrors();
        void openMirrors();
        bool readClosedMirrors();
        void startMoveMirrorsDown();
        void startMoveMirrorsUp();
        void stopMoveMirrors();
        void pcfSetup();
        bool usingMirrors();
        bool readSelectorClosed();

        void voltageCallback();
};
