#include "Carstatus.h"

Carstatus::Carstatus(uint8_t id, const char *name, CanbusMessageType type) : Enum(id, name) {
  this->type = type;

  values[index] = this;
  index++;
};

const Enum* Carstatus::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t Carstatus::getSize() {
  return Carstatus::index;
}

const Enum** Carstatus::getValues() {
  return Carstatus::values;
}

const Enum* Carstatus::values [12] = { 0 };
uint8_t Carstatus::index = 0;
const Carstatus Carstatus::EXTERNAL_TEMPERATURE = Carstatus(0x00, "EXTERNAL_TEMPERATURE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::INTERNAL_TEMPERATURE = Carstatus(0x01, "INTERNAL_TEMPERATURE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::SPEED = Carstatus(0x02, "SPEED", CanbusMessageType::INT);
const Carstatus Carstatus::INTERNAL_LUMINANCE = Carstatus(0x03, "INTERNAL_LUMINANCE", CanbusMessageType::INT);
const Carstatus Carstatus::FRONT_DISTANCE = Carstatus(0x04, "FRONT_DISTANCE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::ENGINE_WATER_COOLING_TEMPERATURE = Carstatus(0x05, "ENGINE_WATER_COOLING_TEMPERATURE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::ENGINE_OIL_TEMPERATURE = Carstatus(0x06, "ENGINE_OIL_TEMPERATURE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::ENGINE_INTAKE_MANIFOLD_PRESSURE = Carstatus(0x07, "ENGINE_INTAKE_MANIFOLD_PRESSURE", CanbusMessageType::FLOAT);
const Carstatus Carstatus::ENGINE_RPM = Carstatus(0x08, "ENGINE_RPM", CanbusMessageType::INT);
const Carstatus Carstatus::TRIP_DURATION = Carstatus(0x09, "TRIP_DURATION", CanbusMessageType::STRING);
const Carstatus Carstatus::TRIP_AVERAGE_SPEED = Carstatus(0x0A, "TRIP_AVERAGE_SPEED", CanbusMessageType::INT);
const Carstatus Carstatus::TRIP_MAX_SPEED = Carstatus(0x0B, "TRIP_MAX_SPEED", CanbusMessageType::INT);
