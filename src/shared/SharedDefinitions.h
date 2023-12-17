#pragma once

#include "Arduino.h"

union ValueType
{
    int intValue;
    float floatValue;
    bool boolValue;
};

#define BAUD_RATE 9600