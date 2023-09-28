#pragma once

#include "Enum.h"
#include "CanbusMessageType.h"
#include "../utils.h"

#define CARSTATUS_SIZE 12

class Carstatus : public Enum {
    public:
        static const Carstatus EXTERNAL_TEMPERATURE;
        static const Carstatus INTERNAL_TEMPERATURE;
        static const Carstatus SPEED;
        static const Carstatus INTERNAL_LUMINANCE;
        static const Carstatus FRONT_DISTANCE;
        static const Carstatus ENGINE_WATER_COOLING_TEMPERATURE;
        static const Carstatus ENGINE_OIL_TEMPERATURE;
        static const Carstatus ENGINE_INTAKE_MANIFOLD_PRESSURE;
        static const Carstatus ENGINE_RPM;
        static const Carstatus TRIP_DURATION;
        static const Carstatus TRIP_AVERAGE_SPEED;
        static const Carstatus TRIP_MAX_SPEED;

        CanbusMessageType type;

        Carstatus() : Enum() {};

        uint16_t getMessageId() {
            uint16_t id = Category::CAR_STATUS.id;
            id = (id << 8) | this->id;
            return id;
        };

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Carstatus::values[i]->id == id) {
                    return Carstatus::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Carstatus::values[i]->name, n) == 0) {
                    return Carstatus::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return Carstatus::index;
        }

        static const Enum** getValues() {
            return Carstatus::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;

        Carstatus(uint8_t id, const char *name, CanbusMessageType type) : Enum(id, name) {
            this->type = type;

            Carstatus::values[Carstatus::index] = this;
            Carstatus::index++;
        };
};

inline const Enum* Carstatus::values [CARSTATUS_SIZE] = { 0 };
inline uint8_t Carstatus::index = 0;
inline const Carstatus Carstatus::EXTERNAL_TEMPERATURE = Carstatus(0x00, "EXTERNAL_TEMPERATURE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::INTERNAL_TEMPERATURE = Carstatus(0x01, "INTERNAL_TEMPERATURE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::SPEED = Carstatus(0x02, "SPEED", CanbusMessageType::INT);
inline const Carstatus Carstatus::INTERNAL_LUMINANCE = Carstatus(0x03, "INTERNAL_LUMINANCE", CanbusMessageType::INT);
inline const Carstatus Carstatus::FRONT_DISTANCE = Carstatus(0x04, "FRONT_DISTANCE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::ENGINE_WATER_COOLING_TEMPERATURE = Carstatus(0x05, "ENGINE_WATER_COOLING_TEMPERATURE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::ENGINE_OIL_TEMPERATURE = Carstatus(0x06, "ENGINE_OIL_TEMPERATURE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::ENGINE_INTAKE_MANIFOLD_PRESSURE = Carstatus(0x07, "ENGINE_INTAKE_MANIFOLD_PRESSURE", CanbusMessageType::FLOAT);
inline const Carstatus Carstatus::ENGINE_RPM = Carstatus(0x08, "ENGINE_RPM", CanbusMessageType::INT);
inline const Carstatus Carstatus::TRIP_DURATION = Carstatus(0x09, "TRIP_DURATION", CanbusMessageType::STRING);
inline const Carstatus Carstatus::TRIP_AVERAGE_SPEED = Carstatus(0x0A, "TRIP_AVERAGE_SPEED", CanbusMessageType::INT);
inline const Carstatus Carstatus::TRIP_MAX_SPEED = Carstatus(0x0B, "TRIP_MAX_SPEED", CanbusMessageType::INT);
