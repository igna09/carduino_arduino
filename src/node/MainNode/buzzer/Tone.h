#pragma once

enum class ToneType : uint8_t {
    INFO,
    WARNING,
    ERROR
};

struct ToneProfile {
    uint32_t freqHz;
    uint16_t onMs;
    uint16_t gapMs;
    uint8_t  count;
};

// Frequenze/durate/impulsi inventati, coerenti con la logica originale (severity crescente = più acuto, più impulsi, più veloce)
inline const ToneProfile& getToneProfile(ToneType type) {
    static constexpr ToneProfile profiles[] = {
        { 523, 250, 0,   1 },  // INFO:    C5, singolo beep breve e morbido
        { 660, 150, 200, 2 },  // WARNING: E5, doppio beep (come il MEDIUM originale)
        { 880, 100, 75,  6 },  // ERROR:   A5, sestupla rapida e insistente (come HIGH originale)
    };
    return profiles[static_cast<uint8_t>(type)];
}