#pragma once

#include  <Arduino.h>
#include "Enum.h"

class KlineValueEnum : public Enum {
    public: 
      static const KlineValueEnum DEGREE;
      char *unit;

      static const Enum* getValueById(uint8_t id);
      static const Enum** getValues();
      static uint8_t getSize();

    private: 
      static const Enum* values[];
      static uint8_t index;

      KlineValueEnum(uint8_t id, char *name, char *unit);
}; 
