#pragma once

enum class ToneType : uint8_t {
    INFO,
    WARNING,
    ERROR,
    MODE_ENTER,
    MODE_EXIT
};

struct ToneProfile {
    uint32_t freqHz;
    uint16_t onMs;
    uint16_t gapMs;
    uint8_t  count;
};

inline const ToneProfile& getToneProfile(ToneType type) {
    static constexpr ToneProfile profiles[] = {
        { 523, 250, 0,   1 },  // INFO:       C5, singolo beep breve e morbido
        { 660, 150, 200, 2 },  // WARNING:    E5, doppio beep
        { 880, 100, 75,  6 },  // ERROR:      A5, sestupla rapida e insistente
        { 784, 80,  60,  2 },  // MODE_ENTER: G5, doppio beep breve ascendente (percettivo, non urgente)
        { 392, 120, 0,   1 },  // MODE_EXIT:  G4, singolo beep grave e più lungo (chiusura)
    };
    return profiles[static_cast<uint8_t>(type)];
}
