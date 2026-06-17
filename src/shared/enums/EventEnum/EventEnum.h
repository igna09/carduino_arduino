#pragma once

#include <Arduino.h>
#include "../Enum.h" // Includiamo la tua classe base
#include "DataTypeEnum.h"

#define EVENT_SIZE 46
#define MAX_VALUES_PER_PAYLOAD 8

class EventEnum : public Enum {
    public:
        static const EventEnum ENABLE;
        static const EventEnum ENABLE_INTERRUPT;
        static const EventEnum ENABLE_FINISH;
        static const EventEnum DISABLE;
        static const EventEnum DISABLE_INTERRUPT;
        static const EventEnum DISABLE_FINISH;
        static const EventEnum GET_HELLOS;
        static const EventEnum HELLO;
        static const EventEnum HEARTBEAT;
        static const EventEnum LOCK_CAR;
        static const EventEnum UNLOCK_CAR;
        static const EventEnum TEST;
        static const EventEnum RESET_WEBAPP;
        static const EventEnum BLE_PAIRING_CODE;
        static const EventEnum RESTART;
        static const EventEnum SWC_PAIR;
        static const EventEnum GET_SETTINGS;
        static const EventEnum WARNING_SEVERITY_LOW;
        static const EventEnum WARNING_SEVERITY_MEDIUM;
        static const EventEnum WARNING_SEVERITY_HIGH;

        static const EventEnum EXTERNAL_TEMPERATURE;
        static const EventEnum INTERNAL_TEMPERATURE;
        static const EventEnum SPEED;
        static const EventEnum INTERNAL_LUMINANCE;
        static const EventEnum FRONT_DISTANCE;
        static const EventEnum ENGINE_WATER_COOLING_TEMPERATURE;
        static const EventEnum ENGINE_OIL_TEMPERATURE;
        static const EventEnum ENGINE_INTAKE_MANIFOLD_PRESSURE;
        static const EventEnum ENGINE_RPM;
        static const EventEnum TRIP_DURATION;
        static const EventEnum TRIP_AVERAGE_SPEED;
        static const EventEnum TRIP_MAX_SPEED;
        static const EventEnum INJECTED_QUANTITY;
        static const EventEnum FUEL_CONSUMPTION;
        static const EventEnum BATTERY_VOLTAGE;
        static const EventEnum IS_REVERSE;
        static const EventEnum IS_KEY_ON;

        static const EventEnum VOLUME_UP;
        static const EventEnum VOLUME_DOWN;
        static const EventEnum PLAY_PAUSE;
        static const EventEnum NEXT;
        static const EventEnum LONG_PRESS;

        static const EventEnum TIME_SYNC_REQUEST;
        static const EventEnum TIME_SYNC_RESPONSE;

        static const EventEnum WRITE_SETTING;
        static const EventEnum READ_SETTING;

        DataTypeEnum types[MAX_VALUES_PER_PAYLOAD];
        uint8_t valueTypesSize;

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(EventEnum::values[i]->id == id) {
                    return EventEnum::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(EventEnum::values[i]->name, n) == 0) {
                    return EventEnum::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return EventEnum::index;
        }

        static const Enum** getValues() {
            return EventEnum::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;
        
        EventEnum(uint8_t id, const char *name, const DataTypeEnum* inputTypes, uint8_t inputSize) : Enum(id, name) {
            uint8_t totalBytes = 0;

            valueTypesSize = inputSize > MAX_VALUES_PER_PAYLOAD ? MAX_VALUES_PER_PAYLOAD : inputSize;

            for (uint8_t i = 0; i < MAX_VALUES_PER_PAYLOAD; i++) {
                if (i < inputSize) {
                    this->types[i] = inputTypes[i];
                    totalBytes += inputTypes[i].size;
                } else {
                    // Riempimento automatico con TYPE_END per il resto dell'array
                    this->types[i] = DataTypeEnum::NONE;
                }
            }

            // Blocco di sicurezza hardware: se i tipi superano gli 8 byte totali, solleva un errore di runtime/debug
            if (totalBytes > 8) {
                Serial.print(F("CRITICAL ERROR: Event "));
                Serial.print(name);
                Serial.println(F(" exceeds 8 bytes payload limit!"));
                // Opzionale: puoi mandare in loop infinito il pin di blocco o lampeggiare un led di errore
            }

            EventEnum::values[EventEnum::index] = this;
            EventEnum::index++;
        };

        EventEnum(uint8_t id, const char *name) : Enum(id, name) {
            uint8_t totalBytes = 0;

            for (uint8_t i = 0; i < MAX_VALUES_PER_PAYLOAD; i++) {
                this->types[i] = DataTypeEnum::NONE;
            }
            valueTypesSize = 0;

            EventEnum::values[EventEnum::index] = this;
            EventEnum::index++;
        };
};

inline const Enum* EventEnum::values [EVENT_SIZE] = { 0 };
inline uint8_t EventEnum::index = 0;

static const DataTypeEnum _t_float[]  = {DataTypeEnum::FLOAT};
static const DataTypeEnum _t_uint8[]  = {DataTypeEnum::UINT8};
static const DataTypeEnum _t_uint16[] = {DataTypeEnum::UINT16};
static const DataTypeEnum _t_uint32[] = {DataTypeEnum::UINT32};
static const DataTypeEnum _t_int32[]  = {DataTypeEnum::INT32};
static const DataTypeEnum _t_bool[]   = {DataTypeEnum::BOOL};
static const DataTypeEnum _t_write_setting[] = {DataTypeEnum::UINT8, DataTypeEnum::UINT32};

inline const EventEnum EventEnum::ENABLE = EventEnum(0x00, "ENABLE");
inline const EventEnum EventEnum::ENABLE_INTERRUPT = EventEnum(0x01, "ENABLE_INTERRUPT");
inline const EventEnum EventEnum::ENABLE_FINISH = EventEnum(0x02, "ENABLE_FINISH");
inline const EventEnum EventEnum::DISABLE = EventEnum(0x03, "DISABLE");
inline const EventEnum EventEnum::DISABLE_INTERRUPT = EventEnum(0x04, "DISABLE_INTERRUPT");
inline const EventEnum EventEnum::DISABLE_FINISH = EventEnum(0x05, "DISABLE_FINISH");
inline const EventEnum EventEnum::GET_HELLOS = EventEnum(0x06, "GET_HELLOS");
inline const EventEnum EventEnum::HELLO = EventEnum(0x07, "HELLO");
inline const EventEnum EventEnum::HEARTBEAT = EventEnum(0x08, "HEARTBEAT");
inline const EventEnum EventEnum::LOCK_CAR = EventEnum(0x09, "LOCK_CAR");
inline const EventEnum EventEnum::UNLOCK_CAR = EventEnum(0x0A, "UNLOCK_CAR");
inline const EventEnum EventEnum::TEST = EventEnum(0x0B, "TEST");
inline const EventEnum EventEnum::RESET_WEBAPP = EventEnum(0x0C, "RESET_WEBAPP");
inline const EventEnum EventEnum::BLE_PAIRING_CODE = EventEnum(0x0D, "BLE_PAIRING_CODE");
inline const EventEnum EventEnum::RESTART = EventEnum(0x0E, "RESTART");
inline const EventEnum EventEnum::SWC_PAIR = EventEnum(0x0F, "SWC_PAIR");
inline const EventEnum EventEnum::GET_SETTINGS = EventEnum(0x10, "GET_SETTINGS");
inline const EventEnum EventEnum::WARNING_SEVERITY_LOW = EventEnum(0x11, "WARNING_SEVERITY_LOW");
inline const EventEnum EventEnum::WARNING_SEVERITY_MEDIUM = EventEnum(0x12, "WARNING_SEVERITY_MEDIUM");
inline const EventEnum EventEnum::WARNING_SEVERITY_HIGH = EventEnum(0x13, "WARNING_SEVERITY_HIGH");

inline const EventEnum EventEnum::EXTERNAL_TEMPERATURE = EventEnum(0x14, "EXTERNAL_TEMPERATURE", _t_float, 1);
inline const EventEnum EventEnum::INTERNAL_TEMPERATURE = EventEnum(0x15, "INTERNAL_TEMPERATURE", _t_float, 1);
inline const EventEnum EventEnum::SPEED = EventEnum(0x16, "SPEED", _t_uint8, 1);
inline const EventEnum EventEnum::INTERNAL_LUMINANCE = EventEnum(0x17, "INTERNAL_LUMINANCE", _t_uint16, 1);
inline const EventEnum EventEnum::FRONT_DISTANCE = EventEnum(0x18, "FRONT_DISTANCE", _t_float, 1);
inline const EventEnum EventEnum::ENGINE_WATER_COOLING_TEMPERATURE = EventEnum(0x19, "ENGINE_WATER_COOLING_TEMPERATURE", _t_float, 1);
inline const EventEnum EventEnum::ENGINE_OIL_TEMPERATURE = EventEnum(0x1A, "ENGINE_OIL_TEMPERATURE", _t_float, 1);
inline const EventEnum EventEnum::ENGINE_INTAKE_MANIFOLD_PRESSURE = EventEnum(0x1B, "ENGINE_INTAKE_MANIFOLD_PRESSURE", _t_float, 1);
inline const EventEnum EventEnum::ENGINE_RPM = EventEnum(0x1C, "ENGINE_RPM", _t_uint16, 1);
inline const EventEnum EventEnum::TRIP_DURATION = EventEnum(0x1D, "TRIP_DURATION", _t_uint32, 1);
inline const EventEnum EventEnum::TRIP_AVERAGE_SPEED = EventEnum(0x1E, "TRIP_AVERAGE_SPEED", _t_uint8, 1);
inline const EventEnum EventEnum::TRIP_MAX_SPEED = EventEnum(0x1F, "TRIP_MAX_SPEED", _t_uint8, 1);
inline const EventEnum EventEnum::INJECTED_QUANTITY = EventEnum(0x20, "INJECTED_QUANTITY", _t_float, 1);
inline const EventEnum EventEnum::FUEL_CONSUMPTION = EventEnum(0x21, "FUEL_CONSUMPTION", _t_float, 1);
inline const EventEnum EventEnum::BATTERY_VOLTAGE = EventEnum(0x22, "BATTERY_VOLTAGE", _t_float, 1);
inline const EventEnum EventEnum::IS_REVERSE = EventEnum(0x23, "IS_REVERSE", _t_bool, 1);
inline const EventEnum EventEnum::IS_KEY_ON = EventEnum(0x24, "IS_KEY_ON", _t_bool, 1);

inline const EventEnum EventEnum::VOLUME_UP = EventEnum(0x25, "VOLUME_UP");
inline const EventEnum EventEnum::VOLUME_DOWN = EventEnum(0x26, "VOLUME_DOWN");
inline const EventEnum EventEnum::PLAY_PAUSE = EventEnum(0x27, "PLAY_PAUSE");
inline const EventEnum EventEnum::NEXT = EventEnum(0x28, "NEXT");
inline const EventEnum EventEnum::LONG_PRESS = EventEnum(0x29, "LONG_PRESS");

inline const EventEnum EventEnum::TIME_SYNC_REQUEST = EventEnum(0x2A, "TIME_SYNC_REQUEST");
inline const EventEnum EventEnum::TIME_SYNC_RESPONSE = EventEnum(0x2B, "TIME_SYNC_RESPONSE", _t_uint32, 1);

// Payload: [Setting ID (1B)] + [Value Container (4B)] = 6 Byte totali
inline const EventEnum EventEnum::WRITE_SETTING = EventEnum(0x2C, "WRITE_SETTING", _t_write_setting, 2);
inline const EventEnum EventEnum::READ_SETTING = EventEnum(0x2D, "READ_SETTING", _t_write_setting, 2);