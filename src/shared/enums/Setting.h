#pragma once

#include "Enum.h"
#include "CanbusMessageType.h"

class Setting : public Enum {
    public:
        static const Setting *AUTO_CLOSE_REARVIEW_MIRRORS;
        static const Setting *OTA_MODE;

        static const Enum* getValueById(uint8_t id);
        static const Enum** getValues();
        static uint8_t getSize();
        
        const CanbusMessageType *type;

    private:
        static const Enum* values[];
        static uint8_t index;
        
        Setting(uint8_t id, const char *name, const CanbusMessageType *type);
};
