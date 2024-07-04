#pragma once

#include <Arduino.h>

class Enum {
    public:
        const char *name;
        uint8_t id;

        static const Enum** getValues();
        static uint8_t getSize();
        static const Enum* getValueById(uint8_t id);
        
        Enum();
        Enum(uint8_t id, const char *name);

        bool operator== (const Enum &e);
};
