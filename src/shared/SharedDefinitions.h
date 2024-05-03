#pragma once

#include "Arduino.h"

union ValueType
{
    int intValue;
    float floatValue;
    bool boolValue;
};

#define BAUD_RATE 9600

#define HEARTBEAT_INTERVAL 2500
#define HEARTBEAT_INTERVAL_TOLERANCE 200

#define ENCODER_READING_INTERVAL 75

#define SWC_PRESS_INTERVAL  65
#define SWC_PAIRING_INTERVAL  5000
#define SWC_WAITING_PAIRING_INTERVAL  1000
#define SWC_FIRST_WAITING_PAIRING_INTERVAL  5000
#define SWC_PIN_SIZE 8

#define RADIO_TURN_OFF_TIMER 15000
