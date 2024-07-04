#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define EVENT_SIZE 16

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
        static const Event TEST;
        static const Event RESET_WEBAPP;
        static const Event BLE_PAIRING_CODE;
        static const Event RESTART;
        static const Event SWC_PAIR;

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
inline const Event Event::TEST = Event(0x0B, "TEST", &CanbusMessageType::INT);
inline const Event Event::RESET_WEBAPP = Event(0x0C, "RESET_WEBAPP", &CanbusMessageType::INT);
inline const Event Event::BLE_PAIRING_CODE = Event(0x0D, "BLE_PAIRING_CODE", &CanbusMessageType::INT);
inline const Event Event::RESTART = Event(0x0E, "RESTART", &CanbusMessageType::INT);
inline const Event Event::SWC_PAIR = Event(0x0F, "SWC_PAIR", &CanbusMessageType::INT);
