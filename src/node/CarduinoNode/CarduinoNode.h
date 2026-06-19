#pragma once

#include <stdio.h>

class CarduinoNode {
public:
    bool isEnabled;
    CarduinoNode(uint8_t id);
private:
    uint8_t _id;
};
