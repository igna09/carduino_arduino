#pragma once

#include "Enum.h"
#include "Carstatus.h"
#include "KlineEcu.h"
#include "ValueType.h"

#define VALUE_TO_READ_SIZE 8

enum BlockToRead {
    FIRST = 0,
    SECOND = 1,
    THIRD = 2,
    FOURTH = 3
};

class ValueToRead : public Enum {
    public:
        static const ValueToRead ENGINE_RPM;
        static const ValueToRead ENGINE_WATER_COOLING_TEMPERATURE;
        static const ValueToRead SPEED;
        static const ValueToRead INTAKE_PRESSURE;
        static const ValueToRead FUEL_CONSUMPTION;
        static const ValueToRead BATTERY_VOLTAGE;
        static const ValueToRead PEDALS;
        static const ValueToRead CRUISE_BITS;
        static const ValueToRead CRUISE_SYSTEM;
        
        KlineEcu klineEcu;
        uint8_t group;
        uint8_t groupIndex;
        Carstatus carstatus;
        bool send;
        ValueType lastReadValue;
        
        ValueToRead() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(ValueToRead::values[i]->id == id) {
                    return ValueToRead::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(ValueToRead::values[i]->name, n) == 0) {
                    return ValueToRead::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return ValueToRead::index;
        }

        static const Enum** getValues() {
            return ValueToRead::values;
        }

        static KlineEcu** getEcusToRead() {
            KlineEcu **values = new KlineEcu*[VALUE_TO_READ_SIZE];
            uint8_t valuesIndex = 0;
            for(uint8_t i = 0; i < VALUE_TO_READ_SIZE; i++) {
                bool present = false;
                for(uint8_t j = 0; j < valuesIndex && !present; j++) {
                    present = values[j]->id == ((ValueToRead*) ValueToRead::getValues()[i])->klineEcu.id;
                }
                if(!present) {
                    values[valuesIndex] = &((ValueToRead*) ValueToRead::getValues()[i])->klineEcu;
                    valuesIndex++;
                }
            }

            return values;
        }

        static uint8_t getEcusToReadSize() {
            // Allocated on the stack; automatically cleaned up when the function returns
            const KlineEcu* values[VALUE_TO_READ_SIZE]; 
            uint8_t valuesIndex = 0;

            for(uint8_t i = 0; i < VALUE_TO_READ_SIZE; i++) {
                bool present = false;
                for(uint8_t j = 0; j < valuesIndex && !present; j++) {
                    present = values[j]->id == ((ValueToRead*) ValueToRead::getValues()[i])->klineEcu.id;
                }
                if(!present) {
                    values[valuesIndex] = &((ValueToRead*) ValueToRead::getValues()[i])->klineEcu;
                    valuesIndex++;
                }
            }

            return valuesIndex;
        }

        static uint8_t getBlockValuesByEcuSize(KlineEcu ecu) {
            return getBlockValuesByEcuSize((ValueToRead**)ValueToRead::values, VALUE_TO_READ_SIZE, ecu);
        }

        static uint8_t getBlockValuesByEcuSize(ValueToRead** src, uint8_t srcSize, KlineEcu ecu) {
            uint8_t a[255]; // are possible 255 blocks for each ecu

            for(uint8_t i = 0; i < 255; i++) {
                a[i] = 0;
            }

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToRead *v = (ValueToRead*) ValueToRead::values[i];
                if(v->klineEcu.id == ecu.id) {
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

        static uint8_t* getBlockValuesByEcu(ValueToRead** src, uint8_t srcSize, KlineEcu ecu) {
            uint8_t sizeByEcu = getBlockValuesByEcuSize(ecu);
            uint8_t *arrayValues = new uint8_t[sizeByEcu];
            uint8_t counter = 0;

            for(uint8_t i = 0; i < srcSize && counter < sizeByEcu; i++) {
                ValueToRead *v = src[i];
                if(v->klineEcu.id == ecu.id) {
                    bool present = false;
                    for(uint8_t j = 0; j < counter && !present; j++) {
                        present = (arrayValues[j] == v->group);
                    }
                    if(!present) {
                        arrayValues[counter++] = v->group;
                    }
                }
            }

            return arrayValues;
        }

        static uint8_t* getBlockValuesByEcu(KlineEcu ecu) {
            return getBlockValuesByEcu((ValueToRead**)ValueToRead::values, VALUE_TO_READ_SIZE, ecu);
        }
        
        static uint8_t getValuesByEcuBlockSize(ValueToRead** src, uint8_t srcSize, KlineEcu ecu, uint8_t block) {
            uint8_t counter = 0;

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToRead *v = (ValueToRead*) src[i];
                if(v->klineEcu.id == ecu.id && v->group == block) {
                    counter++;
                }
            }
            
            return counter;
        }
        
        static uint8_t getValuesByEcuBlockSize(KlineEcu ecu, uint8_t block) {
            return getValuesByEcuBlockSize((ValueToRead**)ValueToRead::values, VALUE_TO_READ_SIZE, ecu, block);
        }
        
        static ValueToRead** getValuesByEcuBlock(ValueToRead** src, uint8_t srcSize, KlineEcu ecu, uint8_t block) {
            uint8_t sizeByEcuBlock = getValuesByEcuBlockSize(src, srcSize, ecu, block);
            uint8_t counter = 0;
            ValueToRead** arrayValues = new ValueToRead*[sizeByEcuBlock];

            for(uint8_t i = 0; i < srcSize; i++) {
                ValueToRead *v = (ValueToRead*) src[i];
                if(v->klineEcu.id == ecu.id && v->group == block) {
                    arrayValues[counter] = v;
                    counter++;
                }
            }
            
            return arrayValues;
        }
        
        static ValueToRead** getValuesByEcuBlock(KlineEcu ecu, uint8_t block) {
            uint8_t sizeByEcuBlock = getValuesByEcuBlockSize(ecu, block);
            uint8_t counter = 0;
            ValueToRead** arrayValues = new ValueToRead*[sizeByEcuBlock];

            for(uint8_t i = 0; i < VALUE_TO_READ_SIZE && counter < sizeByEcuBlock; i++) {
                ValueToRead *v = (ValueToRead*) ValueToRead::values[i];
                if(v->klineEcu.id == ecu.id && v->group == block) {
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

        ValueToRead(uint8_t id, const char *name, KlineEcu ecu, uint8_t group, uint8_t groupIndex, Carstatus carstatus, bool send) : Enum(id, name) {
            this->klineEcu = ecu;
            this->group = group;
            this->groupIndex = groupIndex;
            this->carstatus = carstatus;
            this->send = send;
            
            ValueToRead::values[ValueToRead::index] = this;
            ValueToRead::index++;
        };
};

inline const Enum* ValueToRead::values[VALUE_TO_READ_SIZE] = { 0 };
inline uint8_t ValueToRead::index = 0;
inline const ValueToRead ValueToRead::ENGINE_RPM = ValueToRead(0x00, "ENGINE_RPM", KlineEcu::ENGINE, 1, BlockToRead::FIRST, Carstatus::ENGINE_RPM, true);
inline const ValueToRead ValueToRead::ENGINE_WATER_COOLING_TEMPERATURE = ValueToRead(0x01, "ENGINE_WATER_COOLING_TEMPERATURE", KlineEcu::ENGINE, 2, BlockToRead::FOURTH, Carstatus::ENGINE_WATER_COOLING_TEMPERATURE, true);
inline const ValueToRead ValueToRead::SPEED = ValueToRead(0x02, "SPEED", KlineEcu::ENGINE, 6, BlockToRead::FIRST, Carstatus::SPEED, true);
inline const ValueToRead ValueToRead::INTAKE_PRESSURE = ValueToRead(0x03, "INTAKE_PRESSURE", KlineEcu::ENGINE, 11, BlockToRead::THIRD, Carstatus::ENGINE_INTAKE_MANIFOLD_PRESSURE, true);
inline const ValueToRead ValueToRead::FUEL_CONSUMPTION = ValueToRead(0x04, "FUEL_CONSUMPTION", KlineEcu::ENGINE, 15, BlockToRead::THIRD, Carstatus::FUEL_CONSUMPTION, false);
inline const ValueToRead ValueToRead::BATTERY_VOLTAGE = ValueToRead(0x05, "BATTERY_VOLTAGE", KlineEcu::ENGINE, 12, BlockToRead::THIRD, Carstatus::BATTERY_VOLTAGE, true);
inline const ValueToRead ValueToRead::PEDALS = ValueToRead(0x06, "PEDALS", KlineEcu::ENGINE, 6, BlockToRead::SECOND, Carstatus::PEDALS, false);
inline const ValueToRead ValueToRead::CRUISE_BITS = ValueToRead(0x07, "CRUISE_BITS", KlineEcu::ENGINE, 22, BlockToRead::SECOND, Carstatus::CRUISE_BITS, false);
inline const ValueToRead ValueToRead::CRUISE_SYSTEM = ValueToRead(0x08, "CRUISE_SYSTEM", KlineEcu::ENGINE, 6, BlockToRead::FOURTH, Carstatus::CRUISE_SYSTEM, false);
