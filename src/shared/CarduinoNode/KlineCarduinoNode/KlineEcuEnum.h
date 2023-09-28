#pragma once

#include "../../enums/Enum.h"

class KlineEcuEnum : public Enum {
    public:
        static const KlineEcuEnum INSTRUMENT;
        static const KlineEcuEnum ENGINE;
        
        uint8_t address;
        unsigned int baud;
        
        KlineEcuEnum() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(KlineEcuEnum::values[i]->id == id) {
                    return KlineEcuEnum::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(KlineEcuEnum::values[i]->name, n) == 0) {
                    return KlineEcuEnum::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return KlineEcuEnum::index;
        }

        static const Enum** getValues() {
            return KlineEcuEnum::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;

        KlineEcuEnum(uint8_t id, const char *name, uint8_t address, unsigned long baud) : Enum(id, name) {
            this->address = address;
            this->baud = baud;
            
            KlineEcuEnum::values[KlineEcuEnum::index] = this;
            KlineEcuEnum::index++;
        };
};

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* KlineEcuEnum::values [2] = { 0 };
inline uint8_t KlineEcuEnum::index = 0;
inline const KlineEcuEnum KlineEcuEnum::ENGINE = KlineEcuEnum(0x00, "ENGINE", 0x01, 9600);
inline const KlineEcuEnum KlineEcuEnum::INSTRUMENT = KlineEcuEnum(0x01, "INSTRUMENT", 0x17, 10400);
