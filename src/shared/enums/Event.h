#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define EVENT_SIZE 9

/**
 * these events send as payload sender id
*/

class Event : public TypedEnum {
    public:
        static const Event TURN_ON;
        static const Event TURN_ON_INTERRUPT;
        static const Event TURN_ON_FINISH;
        static const Event TURN_OFF;
        static const Event TURN_OFF_INTERRUPT;
        static const Event TURN_OFF_FINISH;
        static const Event GET_HELLOS;
        static const Event HELLO;
        static const Event HEARTBEAT;

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
inline const Event Event::TURN_ON = Event(0x00, "TURN_ON", &CanbusMessageType::INT);
inline const Event Event::TURN_ON_INTERRUPT = Event(0x01, "TURN_ON_INTERRUPT", &CanbusMessageType::INT);
inline const Event Event::TURN_ON_FINISH = Event(0x02, "TURN_ON_FINISH", &CanbusMessageType::INT);
inline const Event Event::TURN_OFF = Event(0x03, "TURN_OFF", &CanbusMessageType::INT);
inline const Event Event::TURN_OFF_INTERRUPT = Event(0x04, "TURN_OFF_INTERRUPT", &CanbusMessageType::INT);
inline const Event Event::TURN_OFF_FINISH = Event(0x05, "TURN_OFF_FINISH", &CanbusMessageType::INT);
inline const Event Event::HELLO = Event(0x06, "HELLO", &CanbusMessageType::INT); //contains id sending hello (heartbeat and events used to get a list of nodes)
inline const Event Event::GET_HELLOS = Event(0x07, "GET_HELLOS", &CanbusMessageType::BOOL); //request all hellos messages
inline const Event Event::HEARTBEAT = Event(0x08, "HEARTBEAT", &CanbusMessageType::INT);
