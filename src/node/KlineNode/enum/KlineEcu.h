#pragma once

#include "Enum.h"

#define KLINE_ECU_SIZE 2

class KlineEcu : public Enum {
    public:
        static const KlineEcu INSTRUMENT;
        static const KlineEcu ENGINE;
        
        uint8_t address;
        unsigned int baud;
        
        KlineEcu() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(KlineEcu::values[i]->id == id) {
                    return KlineEcu::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(KlineEcu::values[i]->name, n) == 0) {
                    return KlineEcu::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return KlineEcu::index;
        }

        static const Enum** getValues() {
            return KlineEcu::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;

        KlineEcu(uint8_t id, const char *name, uint8_t address, unsigned long baud) : Enum(id, name) {
            this->address = address;
            this->baud = baud;
            
            KlineEcu::values[KlineEcu::index] = this;
            KlineEcu::index++;
        };
};

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* KlineEcu::values [KLINE_ECU_SIZE] = { 0 };
inline uint8_t KlineEcu::index = 0;
inline const KlineEcu KlineEcu::ENGINE = KlineEcu(0x00, "ENGINE", 0x01, 9600);
inline const KlineEcu KlineEcu::INSTRUMENT = KlineEcu(0x01, "INSTRUMENT", 0x17, 10400);
