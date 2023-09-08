#pragma once

#define FOREACH_CARSTATUS(ACTION) \
        ACTION(EXTERNAL_TEMPERATURE)   \
        ACTION(INTERNAL_TEMPERATURE)  \
        ACTION(SPEED)  \
        ACTION(INTERNAL_LUMINANCE)  \
        ACTION(FRONT_DISTANCE)  \
        ACTION(ENGINE_WATER_COOLING_TEMPERATURE)  \
        ACTION(ENGINE_OIL_TEMPERATURE)  \
        ACTION(ENGINE_INTAKE_MANIFOLD_PRESSURE)  \
        ACTION(ENGINE_RPM)  \
        ACTION(TRIP_DURATION)  \
        ACTION(TRIP_AVERAGE_SPEED)  \
        ACTION(TRIP_MAX_SPEED)  \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

enum Carstatus {
    FOREACH_CARSTATUS(GENERATE_ENUM)
};

static const char *CARSTATUS_STRING[] = {
    FOREACH_CARSTATUS(GENERATE_STRING)
};
