#pragma once

#include "Enum.h"

#define CANBUS_MESSAGE_TYPE_SIZE 4

class CanbusMessageType : public Enum {
    public:
        static const CanbusMessageType INT;
        static const CanbusMessageType FLOAT;
        static const CanbusMessageType STRING;
        static const CanbusMessageType BOOL;

        CanbusMessageType() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(CanbusMessageType::values[i]->id == id) {
                    return CanbusMessageType::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(CanbusMessageType::values[i]->name, n) == 0) {
                    return CanbusMessageType::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return CanbusMessageType::index;
        }

        static const Enum** getValues() {
            return CanbusMessageType::values;
        }


    private:
        static const Enum* values[];
        static uint8_t index;

        CanbusMessageType(uint8_t id, const char *name) : Enum(id, name) {
            CanbusMessageType::values[CanbusMessageType::index] = this;
            CanbusMessageType::index++;
        };
};

inline const Enum* CanbusMessageType::values [CANBUS_MESSAGE_TYPE_SIZE] = { 0 };
inline uint8_t CanbusMessageType::index = 0;
inline const CanbusMessageType CanbusMessageType::INT = CanbusMessageType(0x00, "INT");
inline const CanbusMessageType CanbusMessageType::FLOAT = CanbusMessageType(0x01, "FLOAT");
inline const CanbusMessageType CanbusMessageType::STRING = CanbusMessageType(0x02, "STRING");
inline const CanbusMessageType CanbusMessageType::BOOL = CanbusMessageType(0x03, "BOOL");
