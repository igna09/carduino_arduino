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
