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

#define ENCODER_READING_INTERVAL 100

#define SWC_PRESS_INTERVAL  90
#define SWC_PAIRING_INTERVAL  2500
#define SWC_PIN_SIZE 8
