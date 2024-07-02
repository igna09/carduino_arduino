#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define EVENT_SIZE 11

/**
 * these events send as payload sender id
*/

class Event : public TypedEnum {
    public:
        static const Event ENABLE;
        static const Event ENABLE_INTERRUPT;
        static const Event ENABLE_FINISH;
        static const Event DISABLE;
        static const Event DISABLE_INTERRUPT;
        static const Event DISABLE_FINISH;
        static const Event GET_HELLOS;
        static const Event HELLO;
        static const Event HEARTBEAT;
        static const Event LOCK_CAR;
        static const Event UNLOCK_CAR;
        static const Event ENABLE_NEW_BLE_PAIRING;
        static const Event DISABLE_NEW_BLE_PAIRING;

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
inline const Event Event::ENABLE = Event(0x00, "ENABLE", &CanbusMessageType::INT);
inline const Event Event::ENABLE_INTERRUPT = Event(0x01, "ENABLE_INTERRUPT", &CanbusMessageType::INT);
inline const Event Event::ENABLE_FINISH = Event(0x02, "ENABLE_FINISH", &CanbusMessageType::INT);
inline const Event Event::DISABLE = Event(0x03, "DISABLE", &CanbusMessageType::INT);
inline const Event Event::DISABLE_INTERRUPT = Event(0x04, "DISABLE_INTERRUPT", &CanbusMessageType::INT);
inline const Event Event::DISABLE_FINISH = Event(0x05, "DISABLE_FINISH", &CanbusMessageType::INT);
inline const Event Event::HELLO = Event(0x06, "HELLO", &CanbusMessageType::INT); //contains id sending hello (heartbeat and events used to get a list of nodes)
inline const Event Event::GET_HELLOS = Event(0x07, "GET_HELLOS", &CanbusMessageType::INT); //request all hellos messages
inline const Event Event::HEARTBEAT = Event(0x08, "HEARTBEAT", &CanbusMessageType::INT);
inline const Event Event::LOCK_CAR = Event(0x09, "LOCK_CAR", &CanbusMessageType::INT);
inline const Event Event::UNLOCK_CAR = Event(0x0A, "UNLOCK_CAR", &CanbusMessageType::INT);
inline const Event Event::ENABLE_NEW_BLE_PAIRING = Event(0x09, "ENABLE_NEW_BLE_PAIRING", &CanbusMessageType::INT);
inline const Event Event::DISABLE_NEW_BLE_PAIRING = Event(0x0A, "DISABLE_NEW_BLE_PAIRING", &CanbusMessageType::INT);
