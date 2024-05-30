#pragma once

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeEvent/DoorNodeEvent.h"
#include "shared/CarduinoNode/DoorCarduinoNode/executors/DoorNodeCarstatus/DoorNodeCarstatus.h"
#include "Versatile_RotaryEncoder.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include <PCF8574.h>

#define PIN_MIRROR_A P0
#define PIN_MIRROR_B P1
#define PIN_MIRROR_C P2
#define PIN_MIRROR_D P3
#define PIN_CLOSED_MIRRORS_RELAY P7 // CHECK IF MIRROR IS CLOSED (MIRROR RELAY)
#define PIN_OPEN_MIRRORS P4 // PIN TO CONTROL OPENING/CLOSING MIRRORS
#define PIN_MIRROR_SELECTOR_ON_CLOSED P5 // PIN TO CHECK IF SELECTOR IS ON CLOSED
#define MIRRORS_MOVING_TIME 2500

#define VOLTAGE_READING_INTERVAL 5000
#define VOLTAGE_READING_PIN A0
#define VOLTAGE_READING_PIN_RESISTOR 1360000 //1.36 MOhm
#define VOLTAGE_READING_PIN_COMPENSATION 0.987

class DoorCarduinoNode : public CarduinoNode {
    public:
        DoorCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);

        PCF8574 *pcf8574;
        bool closedMirrors;
        bool movingMirrors;
        Task *stopMoveMirrorsTask;
        Event *lastReceivedEvent;
        bool reverse;
        bool mirrorSelectorOnClosed;

        void loop();
        void closeMirrors();
        void openMirrors();
        bool readClosedMirrors();
        void startMoveMirrorsDown();
        void startMoveMirrorsUp();
        void stopMoveMirrors();
        void pcfSetup();
        bool usingMirrors();
        bool readSelectorClosed();
        void turnOn() override;
        void turnOff() override;
        void turnOffInterrupt() override;
        void turnOnInterrupt() override;

        void voltageCallback();
};
