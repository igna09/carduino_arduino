#pragma once

#include <Arduino.h>
#include "KlineEcuEnum.h"
#include "../../enums/Enum.h"

class ValueToReadEnum : Enum {
    public:
        static const ValueToReadEnum VALUE_TO_READ_0;
        static const ValueToReadEnum VALUE_TO_READ_1;
        static const ValueToReadEnum VALUE_TO_READ_2;
        static const ValueToReadEnum VALUE_TO_READ_3;

        uint8_t value;

        ValueToReadEnum(){};

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

    private:
        static const Enum* values[];
        static uint8_t index;
        
        ValueToReadEnum(uint8_t id, const char *name, uint8_t v) : Enum(id, name) {
            value = v;
            
            ValueToReadEnum::values[ValueToReadEnum::index] = this;
            ValueToReadEnum::index++;
        };
};

inline const Enum* ValueToReadEnum::values [4] = { 0 };
inline uint8_t ValueToReadEnum::index = 0;
inline const ValueToReadEnum ValueToReadEnum::VALUE_TO_READ_0 = ValueToReadEnum(0x00, "VALUE_TO_READ_0", 0);
inline const ValueToReadEnum ValueToReadEnum::VALUE_TO_READ_1 = ValueToReadEnum(0x01, "VALUE_TO_READ_1", 1);
inline const ValueToReadEnum ValueToReadEnum::VALUE_TO_READ_2 = ValueToReadEnum(0x02, "VALUE_TO_READ_2", 2);
inline const ValueToReadEnum ValueToReadEnum::VALUE_TO_READ_3 = ValueToReadEnum(0x03, "VALUE_TO_READ_3", 3);

class ValueToRead {
    public:
        ValueToReadEnum valueToReadEnum;
        std::function<void(float)> floatCallback;
        std::function<void(char*)> charCallback;
        ValueToRead(ValueToReadEnum valueToReadEnum, std::function<void(float)> callback){
            this->valueToReadEnum = valueToReadEnum;
            this->floatCallback = callback;
        };
        ValueToRead(ValueToReadEnum valueToReadEnum, std::function<void(char*)> callback){
            this->valueToReadEnum = valueToReadEnum;
            this->charCallback = callback;
        };
        ValueToRead(){};
};

class BlockToRead {
    public:
        uint8_t block;
        ValueToRead valuesToRead[4];
        uint8_t size = 0;
        BlockToRead(){};
        void addValueToRead(ValueToRead v){
            valuesToRead[size] = v;
            size++;
        };
};

class EcuToRead {
    public:
        KlineEcuEnum ecu;
        BlockToRead blocksToRead[16];
        uint8_t size = 0;
        EcuToRead(){};
        void addBlockToRead(BlockToRead b){
            blocksToRead[size] = b;
            size++;
        };
};

class EcusToRead {
    public:
        EcuToRead ecusToRead[8];
        uint8_t size = 0;
        EcusToRead(){};
        void addEcuToRead(EcuToRead e){
            ecusToRead[size] = e;
            size++;
        };
};
