#pragma once

#include "Enum.h"

class CanbusMessageType : public Enum {
    public:
        static const CanbusMessageType INT;
        static const CanbusMessageType FLOAT;
        static const CanbusMessageType STRING;
        static const CanbusMessageType BOOL;

        static const Enum* getValueById(uint8_t id);
        static const Enum** getValues();
        static uint8_t getSize();
        
        CanbusMessageType();
        CanbusMessageType(uint8_t id, const char *name);

    private:
        static const Enum* values[];
        static uint8_t index;
};

