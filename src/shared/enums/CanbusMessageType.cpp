#include "CanbusMessageType.h"

CanbusMessageType::CanbusMessageType() : Enum(0, nullptr) {};
CanbusMessageType::CanbusMessageType(uint8_t id, const char *name) : Enum(id, name) {
  values[index] = this;
  index++;
};

const Enum* CanbusMessageType::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t CanbusMessageType::getSize() {
  return index;
}

const Enum** CanbusMessageType::getValues() {
  return values;
}

const Enum* CanbusMessageType::values [4] = { 0 };
uint8_t CanbusMessageType::index = 0;
const CanbusMessageType CanbusMessageType::INT = CanbusMessageType(0x00, "INT");
const CanbusMessageType CanbusMessageType::STRING = CanbusMessageType(0x01, "FLOAT");
const CanbusMessageType CanbusMessageType::FLOAT = CanbusMessageType(0x02, "STRING");
const CanbusMessageType CanbusMessageType::BOOL = CanbusMessageType(0x02, "BOOL");
