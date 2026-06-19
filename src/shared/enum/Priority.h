#pragma once

#include <cstring>

#include "Enum.h"

// Modifica questo numero in base a quanti nodi hai
#define CANBUS_PRIORITY_SIZE 2

class Priority : public Enum {
    public:
        // 1. Definisci i nodi qui (sostituiscono i vecchi #define)
        static const Priority L;
        static const Priority H;

        Priority() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Priority::values[i]->id == id) {
                    return Priority::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Priority::values[i]->name, n) == 0) {
                    return Priority::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() { return Priority::index; }
        static const Enum** getValues() { return Priority::values; }

    private:
        Priority(uint8_t id, const char *name) : Enum(id, name) {
            Priority::values[Priority::index] = this;
            Priority::index++;
        };

        static const Enum* values[];
        static uint8_t index;
};

// 2. Inizializzazione unica dei valori e degli ID numerici
inline const Enum* Priority::values[CANBUS_PRIORITY_SIZE] = { 0 };
inline uint8_t Priority::index = 0;

inline const Priority Priority::H = Priority(0x00, "H");
inline const Priority Priority::L = Priority(0x01, "L");