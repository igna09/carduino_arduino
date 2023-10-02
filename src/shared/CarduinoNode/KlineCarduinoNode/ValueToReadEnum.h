#pragma once

#include <Arduino.h>
#include "../../enums/Enum.h"
#include "../../enums/Carstatus.h"
#include "KlineEcuEnum.h"

#define VALUE_TO_READ_ENUM_SIZE 2

class ValueToReadEnum : Enum {
    public:
        static const ValueToReadEnum SPEED;
        static const ValueToReadEnum RPM;
        static const ValueToReadEnum ENGINE_WATER_COOLING_TEMPERATURE;
        
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

        static uint8_t getSize() {
            return ValueToReadEnum::index;
        }

        static const Enum** getValues() {
            return ValueToReadEnum::values;
        }

        static uint8_t getBlockValuesByEcuSize(KlineEcuEnum ecu) {
            return getBlockValuesByEcuSize((ValueToReadEnum**)ValueToReadEnum::values, VALUE_TO_READ_ENUM_SIZE, ecu);
        }

        static uint8_t getBlockValuesByEcuSize(ValueToReadEnum** src, uint8_t srcSize, KlineEcuEnum ecu) {
            uint8_t a[255]; // are possible 255 blocks for each ecu

            for(uint8_t i = 0; i < 255; i++) {
                a[i] = 0;
            }

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToReadEnum *v = (ValueToReadEnum*) ValueToReadEnum::values[i];
                if(v->klineEcuEnum.id == ecu.id) {
                    a[v->group]++;
                }
            }

            uint8_t counter = 0;
            for(uint8_t i = 0; i < 255; i++) {
                if(a[i] > 0) {
                    counter++;
                }
            }

            return counter;
        }

        static uint8_t* getBlockValuesByEcu(ValueToReadEnum** src, uint8_t srcSize, KlineEcuEnum ecu) {
            uint8_t sizeByEcu = getBlockValuesByEcuSize(ecu);
            uint8_t counter = 0;
            uint8_t* arrayValues = new uint8_t[sizeByEcu];

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToReadEnum *v = src[i];
                if(v->klineEcuEnum.id == ecu.id) {
                    arrayValues[counter] = v->group;
                    counter++;
                }
            }

            return arrayValues;
        }

        static uint8_t* getBlockValuesByEcu(KlineEcuEnum ecu) {
            return getBlockValuesByEcu((ValueToReadEnum**)ValueToReadEnum::values, VALUE_TO_READ_ENUM_SIZE, ecu);
        }
        
        static uint8_t getValuesByEcuBlockSize(ValueToReadEnum** src, uint8_t srcSize, KlineEcuEnum ecu, uint8_t block) {
            uint8_t counter = 0;

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToReadEnum *v = (ValueToReadEnum*) src[i];
                if(v->klineEcuEnum.id == ecu.id && v->group == block) {
                    counter++;
                }
            }
            
            return counter;
        }
        
        static uint8_t getValuesByEcuBlockSize(KlineEcuEnum ecu, uint8_t block) {
            return getValuesByEcuBlockSize((ValueToReadEnum**)ValueToReadEnum::values, VALUE_TO_READ_ENUM_SIZE, ecu, block);
        }
        
        static ValueToReadEnum** getValuesByEcuBlock(ValueToReadEnum** src, uint8_t srcSize, KlineEcuEnum ecu, uint8_t block) {
            uint8_t sizeByEcuBlock = getValuesByEcuBlockSize(src, srcSize, ecu, block);
            uint8_t counter = 0;
            ValueToReadEnum** arrayValues = new ValueToReadEnum*[sizeByEcuBlock];

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToReadEnum *v = (ValueToReadEnum*) src[i];
                if(v->klineEcuEnum.id == ecu.id && v->group == block) {
                    arrayValues[counter] = v;
                    counter++;
                }
            }
            
            return arrayValues;
        }
        
        static ValueToReadEnum** getValuesByEcuBlock(KlineEcuEnum ecu, uint8_t block) {
            uint8_t sizeByEcuBlock = getValuesByEcuBlockSize(ecu, block);
            uint8_t counter = 0;
            ValueToReadEnum** arrayValues = new ValueToReadEnum*[sizeByEcuBlock];

            for(uint8_t i = 0; i < VALUE_TO_READ_ENUM_SIZE; i++) {
                ValueToReadEnum *v = (ValueToReadEnum*) ValueToReadEnum::values[i];
                if(v->klineEcuEnum.id == ecu.id && v->group == block) {
                    arrayValues[counter] = v;
                    counter++;
                }
            }
            
            return arrayValues;
        }

    private:
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
        };
};

inline const Enum* ValueToReadEnum::values[VALUE_TO_READ_ENUM_SIZE] = { 0 };
inline uint8_t ValueToReadEnum::index = 0;
// inline const ValueToReadEnum ValueToReadEnum::SPEED = ValueToReadEnum(0x00, "SPEED", KlineEcuEnum::ENGINE, 1, 1, Carstatus::ENGINE_RPM);
inline const ValueToReadEnum ValueToReadEnum::RPM = ValueToReadEnum(0x01, "RPM", KlineEcuEnum::ENGINE, 1, 0, Carstatus::ENGINE_RPM);
inline const ValueToReadEnum ValueToReadEnum::ENGINE_WATER_COOLING_TEMPERATURE = ValueToReadEnum(0x01, "ENGINE_WATER_COOLING_TEMPERATURE", KlineEcuEnum::ENGINE, 2, 3, Carstatus::ENGINE_WATER_COOLING_TEMPERATURE);
