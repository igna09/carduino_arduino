#pragma once

#include "Enum.h"

class CarstatusCanbusMessageType : public Enum {
    public:
        static const CarstatusCanbusMessageType INT;
        static const CarstatusCanbusMessageType FLOAT;
        static const CarstatusCanbusMessageType STRING;

        static const Enum* getValueById(uint8_t id);
        static const Enum** getValues();
        static uint8_t getSize();

    private:
        static const Enum* values[];
        static uint8_t index;
        
        CarstatusCanbusMessageType(uint8_t id, char *name);
};

