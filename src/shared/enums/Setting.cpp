#include "Setting.h"

Setting::Setting(uint8_t id, const char *name, const CanbusMessageType *type) : Enum(id, name) {
  this->type = type;
  
  values[index] = this;
  index++;
};

const Enum* Setting::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t Setting::getSize() {
  return index;
}

const Enum** Setting::getValues() {
  return values;
}

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
const Enum* Setting::values [2] = { 0 };
uint8_t Setting::index = 0;
const Setting* Setting::AUTO_CLOSE_REARVIEW_MIRRORS = new Setting(0x00, "AUTO_CLOSE_REARVIEW_MIRRORS", CanbusMessageType::BOOL);
const Setting* Setting::OTA_MODE = new Setting(0x01, "OTA_MODE", CanbusMessageType::BOOL);
