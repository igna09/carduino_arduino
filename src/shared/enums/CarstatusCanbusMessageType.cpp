#include "CarstatusCanbusMessageType.h"

CarstatusCanbusMessageType::CarstatusCanbusMessageType() : Enum(0, nullptr) {};
CarstatusCanbusMessageType::CarstatusCanbusMessageType(uint8_t id, char *name) : Enum(id, name) {
  values[index] = this;
  index++;
};

const Enum* CarstatusCanbusMessageType::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t CarstatusCanbusMessageType::getSize() {
  return index;
}

const Enum** CarstatusCanbusMessageType::getValues() {
  return values;
}

const Enum* CarstatusCanbusMessageType::values [3] = { 0 };
uint8_t CarstatusCanbusMessageType::index = 0;
const CarstatusCanbusMessageType CarstatusCanbusMessageType::INT = CarstatusCanbusMessageType(0x00, "INT");
const CarstatusCanbusMessageType CarstatusCanbusMessageType::STRING = CarstatusCanbusMessageType(0x01, "FLOAT");
const CarstatusCanbusMessageType CarstatusCanbusMessageType::FLOAT = CarstatusCanbusMessageType(0x02, "STRING");
