#pragma once

#include "../../enums/Enum.h"
#include "./ValueToReadEnum.h"

#define KLINE_READ_VALUE_SIZE 2

union ValueType
{
    int intValue;
    float floatValue;
    bool boolValue;
};

class KlineReadValue : public Enum {
    public:
        static const KlineReadValue SPEED;
        static const KlineReadValue FUEL_CONSUMPTION;
        
        uint8_t address;
        unsigned int baud;
        ValueToReadEnum *valueToReadEnum;

        ValueType value;
        
        KlineReadValue() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(KlineReadValue::values[i]->id == id) {
                    return KlineReadValue::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(KlineReadValue::values[i]->name, n) == 0) {
                    return KlineReadValue::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return KlineReadValue::index;
        }

        static const Enum** getValues() {
            return KlineReadValue::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;

        KlineReadValue(uint8_t id, const char *name, ValueToReadEnum *valueToReadEnum) : Enum(id, name) {
            this->address = address;
            this->baud = baud;
            this->valueToReadEnum = valueToReadEnum;
            
            KlineReadValue::values[KlineReadValue::index] = this;
            KlineReadValue::index++;
        };
};

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* KlineReadValue::values [KLINE_READ_VALUE_SIZE] = { 0 };
inline uint8_t KlineReadValue::index = 0;
inline const KlineReadValue KlineReadValue::SPEED = KlineReadValue(0x00, "SPEED", ValueToReadEnum::SPEED);
inline const KlineReadValue KlineReadValue::FUEL_CONSUMPTION = KlineReadValue(0x01, "FUEL_CONSUMPTION", ValueToReadEnum::FUEL_CONSUMPTION);
