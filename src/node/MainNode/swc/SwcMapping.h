#pragma once

enum class SwcPattern : uint8_t {
    SINGLE_CLICK,
    DOUBLE_CLICK,
    LONG_PRESS,
    CW_ROTATION,
    CCW_ROTATION
};

struct SwcMapping {
    uint8_t channel;       // canale fisico SWC (es. pin associato)
    SwcPattern pattern;
    uint8_t eventId;       // Event::XXX.id target
};

// Configurabile: modificabile senza toccare la logica di pairing/detection
static constexpr SwcMapping SWC_MAPPINGS[] = {
    { 0, SwcPattern::SINGLE_CLICK, EV_PLAY_PAUSE },
    { 1, SwcPattern::DOUBLE_CLICK, EV_NEXT },
    { 2, SwcPattern::CW_ROTATION,  EV_VOLUME_UP },
    { 3, SwcPattern::CCW_ROTATION, EV_VOLUME_DOWN }
};
static constexpr uint8_t SWC_MAPPINGS_SIZE = sizeof(SWC_MAPPINGS) / sizeof(SwcMapping);

static const SwcMapping* findSwcMapping(SwcPattern pattern) {
    for (uint8_t i = 0; i < SWC_MAPPINGS_SIZE; i++)
        if (SWC_MAPPINGS[i].pattern == pattern)
            return &SWC_MAPPINGS[i];
    return nullptr;
}