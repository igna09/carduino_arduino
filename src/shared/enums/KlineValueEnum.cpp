#include "KlineValueEnum.h"

KlineValueEnum::KlineValueEnum(uint8_t id, char *name, char *unit) : Enum(id, name) {
  this->name = name;
};

const Enum* KlineValueEnum::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t KlineValueEnum::getSize() {
  return index;
}

const Enum** KlineValueEnum::getValues() {
  return values;
}

const Enum* KlineValueEnum::values [1] = { 0 };
uint8_t KlineValueEnum::index = 0;
const KlineValueEnum KlineValueEnum::DEGREE = KlineValueEnum(0x00, "DEGREE", "°");
