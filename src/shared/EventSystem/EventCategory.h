#pragma once
#include <cstdint>

enum class EventCategory : uint8_t {
    SENSOR,
    CONTROL,
    MEDIA,
    TIME,
    SETTINGS,
    ANY
};

inline const char* eventCategoryToString(EventCategory c) {
    switch (c) {
        case EventCategory::CONTROL:  return "CONTROL";
        // case EventCategory::SENSOR:   return "SENSOR";
        case EventCategory::SENSOR:   return "CAR_STATUS"; // TODO: restore original
        // case EventCategory::MEDIA:    return "MEDIA";
        case EventCategory::MEDIA:    return "MEDIA_CONTROL";
        case EventCategory::TIME:     return "TIME";
        case EventCategory::SETTINGS: return "SETTINGS";
        case EventCategory::ANY:      return "ANY";
        default:                      return "UNKNOWN";
    }
}