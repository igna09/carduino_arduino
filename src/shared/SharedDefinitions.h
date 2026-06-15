#pragma once

#include "Arduino.h"

union ValueType {
    int intValue;
    float floatValue;
    bool boolValue;
};

typedef enum {
    ONLINE,
    OFFLINE
} OnlineEnum;

#define BAUD_RATE 115200

#define HEARTBEAT_INTERVAL 10000
#define HEARTBEAT_INTERVAL_TOLERANCE 150

#define WEMOS_D1_MINI_VOLTAGE_DIVIDER_R1 (220000 * 1.0) //220K OHM multiplyed by 1.0 to keep float precision
#define WEMOS_D1_MINI_VOLTAGE_DIVIDER_R2 (100000 * 1.0) //100K OHM multiplyed by 1.0 to keep float precision

#define ALL_NODES -1

#define BOOL_BYTES_SIZE 1
#define INT_BYTES_SIZE 4
#define FLOAT_BYTES_SIZE 5


#define WEMOS_D1_MINI_SDA D3
#define WEMOS_D1_MINI_SCL SCL
#define ESP32_SDA SDA
#define ESP32_SCL SCL
#if defined(ESP8266)
#define NODE_SDA WEMOS_D1_MINI_SDA
#define NODE_SCL WEMOS_D1_MINI_SCL
#elif defined(ESP32)
#define NODE_SDA ESP32_SDA
#define NODE_SCL ESP32_SCL
#endif

#define SECONDS_TO_MILLISECONDS(seconds) ((seconds) * 1000)
#define SECONDS_TO_MICROSECONDS(seconds) ((seconds) * 1000000)
#define MILLISECONDS_TO_MICROSECONDS(milliseconds) ((milliseconds) * 1000)

#define __VERSION__ 23
