#pragma once

#include <Arduino.h>
#include "../../enums/Enum.h"
#include "KlineEcuEnum.h"
#include "../../enums/Carstatus.h"

#define VALUE_TO_READ_ENUM_SIZE 2

class ValueToReadEnum : Enum {
    public:
        static const ValueToReadEnum SPEED;
        static const ValueToReadEnum RPM;
        
        KlineEcuEnum klineEcuEnum;
        uint8_t group;
        uint8_t groupIndex;
        Carstatus carstatus;
        
        ValueToReadEnum() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(ValueToReadEnum::values[i]->id == id) {
                    return ValueToReadEnum::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(ValueToReadEnum::values[i]->name, n) == 0) {
                    return ValueToReadEnum::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSizeByEcu(KlineEcuEnum ecu) {
            return ValueToReadEnum::indexesPerEcu[ecu.id];
        }

        static const ValueToReadEnum** getValuesByEcu(KlineEcuEnum ecu) {
            return ValueToReadEnum::valuesPerEcu[ecu.id];
        }

        static uint8_t getSize() {
            return ValueToReadEnum::index;
        }

        static const Enum** getValues() {
            return ValueToReadEnum::values;
        }

    private:
        static const ValueToReadEnum** valuesPerEcu[];
        static  int indexesPerEcu[];
        static const Enum* values[];
        static uint8_t index;

        ValueToReadEnum(uint8_t id, const char *name, KlineEcuEnum ecu, uint8_t group, uint8_t groupIndex, Carstatus carstatus) : Enum(id, name) {
            this->klineEcuEnum = ecu;
            this->group = group;
            this->groupIndex = groupIndex;
            this->carstatus = carstatus;
            
            ValueToReadEnum::values[ValueToReadEnum::index] = this;
            ValueToReadEnum::index++;

            ValueToReadEnum::valuesPerEcu[ecu.id][ValueToReadEnum::indexesPerEcu[ecu.id]] = this;
            ValueToReadEnum::indexesPerEcu[ecu.id]++;
        };
};

inline const ValueToReadEnum** ValueToReadEnum::valuesPerEcu[KLINE_ECU_ENUM_SIZE] = { 0 };
inline int ValueToReadEnum::indexesPerEcu[KLINE_ECU_ENUM_SIZE] = { 0 };
inline const Enum* ValueToReadEnum::values[VALUE_TO_READ_ENUM_SIZE] = { 0 };
inline uint8_t ValueToReadEnum::index = 0;
inline const ValueToReadEnum ValueToReadEnum::SPEED = ValueToReadEnum(0x00, "SPEED", KlineEcuEnum::INSTRUMENT, 1, 0, Carstatus::SPEED);
inline const ValueToReadEnum ValueToReadEnum::RPM = ValueToReadEnum(0x01, "RPM", KlineEcuEnum::ENGINE, 1, 0, Carstatus::ENGINE_RPM);
