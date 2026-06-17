#pragma once

#include <Arduino.h>

#include <any>

#include "shared/enums/Enum.h"

#define DATA_TYPE_SIZE 9

class DataTypeEnum : public Enum {
    public:
        static const DataTypeEnum BOOL;
        static const DataTypeEnum UINT8;
        static const DataTypeEnum INT8;
        static const DataTypeEnum UINT16;
        static const DataTypeEnum INT16;
        static const DataTypeEnum UINT32;
        static const DataTypeEnum INT32;
        static const DataTypeEnum FLOAT;
        static const DataTypeEnum NONE;

        uint8_t size;
        std::function<std::any(String)> stringToType;

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(DataTypeEnum::values[i]->id == id) {
                    return DataTypeEnum::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(DataTypeEnum::values[i]->name, n) == 0) {
                    return DataTypeEnum::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return DataTypeEnum::index;
        }

        static const Enum** getValues() {
            return DataTypeEnum::values;
        }

        DataTypeEnum() : Enum() {}
    
    private:
        static const Enum* values[];
        static uint8_t index;
        
        DataTypeEnum(uint8_t id, const char *name, uint8_t size, std::function<std::any(String)> convertFunction) : Enum(id, name) {
            DataTypeEnum::values[DataTypeEnum::index] = this;
            DataTypeEnum::index++;

            stringToType = convertFunction;
            this->size = size;
        }
};

inline const Enum* DataTypeEnum::values [DATA_TYPE_SIZE] = { 0 };
inline uint8_t DataTypeEnum::index = 0;

inline const DataTypeEnum DataTypeEnum::BOOL = DataTypeEnum(0x00, "BOOL", 1, [](String s) -> std::any {
    return (s == "true" || s == "TRUE" || s == "1");
});
inline const DataTypeEnum DataTypeEnum::UINT8 = DataTypeEnum(0x01, "UINT8", 1, [](String s) -> std::any {
    return (uint8_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::INT8 = DataTypeEnum(0x02, "INT8", 1, [](String s) -> std::any {
    return (int8_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::UINT16 = DataTypeEnum(0x03, "UINT16", 2, [](String s) -> std::any {
    return (uint8_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::INT16 = DataTypeEnum(0x04, "INT16", 2, [](String s) -> std::any {
    return (int16_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::UINT32 = DataTypeEnum(0x05, "UINT32", 4, [](String s) -> std::any {
    return (uint32_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::INT32 = DataTypeEnum(0x06, "INT32", 4, [](String s) -> std::any {
    return (int32_t) s.toInt();
});
inline const DataTypeEnum DataTypeEnum::FLOAT = DataTypeEnum(0x07, "FLOAT", 4, [](String s) -> std::any {
    return s.toFloat();
});
inline const DataTypeEnum DataTypeEnum::NONE = DataTypeEnum(0x08, "NONE", 0, [](String s) -> std::any {
    return nullptr;
});