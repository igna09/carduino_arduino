#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define EVENT_SIZE 4

class Event : public TypedEnum {
    public:
        static const Event TURN_ON;
        static const Event TURN_ON_INTERRUPTED;
        static const Event TURN_OFF;
        static const Event TURN_OFF_INTERRUPTED;

        static const TypedEnum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Event::values[i]->id == id) {
                    return Event::values[i];
                }
            }
            return nullptr;
        }

        static const TypedEnum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Event::values[i]->name, n) == 0) {
                    return Event::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return Event::index;
        }

        static const TypedEnum** getValues() {
            return Event::values;
        }

    private:
        static const TypedEnum* values[];
        static uint8_t index;

        
        
        Event(uint8_t id, const char *name, const CanbusMessageType *type) : TypedEnum(id, name, type) {
            Event::values[Event::index] = this;
            Event::index++;
        };
};

inline const TypedEnum* Event::values [EVENT_SIZE] = { 0 };
inline uint8_t Event::index = 0;
inline const Event Event::TURN_ON = Event(0x00, "TURN_ON", &CanbusMessageType::BOOL);
inline const Event Event::TURN_ON_INTERRUPTED = Event(0x01, "TURN_ON_INTERRUPTED", &CanbusMessageType::BOOL);
inline const Event Event::TURN_OFF = Event(0x02, "TURN_OFF", &CanbusMessageType::BOOL);
inline const Event Event::TURN_OFF_INTERRUPTED = Event(0x03, "TURN_OFF_INTERRUPTED", &CanbusMessageType::BOOL);
