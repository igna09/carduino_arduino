#pragma once

#include "Enum.h"
#include "CanbusMessageType.h"

class Carstatus : public Enum {
    public:
        static const Carstatus EXTERNAL_TEMPERATURE;
        static const Carstatus INTERNAL_TEMPERATURE;
        static const Carstatus SPEED;
        static const Carstatus INTERNAL_LUMINANCE;
        static const Carstatus FRONT_DISTANCE;
        static const Carstatus ENGINE_WATER_COOLING_TEMPERATURE;
        static const Carstatus ENGINE_OIL_TEMPERATURE;
        static const Carstatus ENGINE_INTAKE_MANIFOLD_PRESSURE;
        static const Carstatus ENGINE_RPM;
        static const Carstatus TRIP_DURATION;
        static const Carstatus TRIP_AVERAGE_SPEED;
        static const Carstatus TRIP_MAX_SPEED;

        static const Enum* getValueById(uint8_t id);
        static const Enum** getValues();
        static uint8_t getSize();

        CanbusMessageType type;

        Carstatus(uint8_t id, const char *name, CanbusMessageType type);

    private:
        static const Enum* values[];
        static uint8_t index;
};
