#pragma once
#include <cstdint>

enum class EventCategory : uint8_t {
    CONTROL,
    SENSOR,
    MEDIA,
    TIME,
    SETTINGS,
    ANY
};

inline const char* toString(EventCategory c) {
    switch (c) {
        case EventCategory::CONTROL:  return "CONTROL";
        case EventCategory::SENSOR:   return "SENSOR";
        case EventCategory::MEDIA:    return "MEDIA";
        case EventCategory::TIME:     return "TIME";
        case EventCategory::SETTINGS: return "SETTINGS";
        case EventCategory::ANY:      return "ANY";
        default:                      return "UNKNOWN";
    }
}