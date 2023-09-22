#include "Category.h"

Category::Category() : Enum() {}

Category::Category(uint8_t id, const char *name) : Enum(id, name) {
  values[index] = this;
  index++;
};

const Enum* Category::getValueById(uint8_t id) {
  for(uint8_t i = 0; i < getSize(); i++) {
    if(values[i]->id == id) {
      return values[i];
    }
  }
  return nullptr;
}

uint8_t Category::getSize() {
  return index;
}

const Enum** Category::getValues() {
  return values;
}

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
const Enum* Category::values [4] = { 0 };
uint8_t Category::index = 0;
const Category* Category::CAR_STATUS = new Category(0x00, "CAR_STATUS");
const Category* Category::READ_SETTINGS = new Category(0x01, "READ_SETTINGS");
const Category* Category::MEDIA_CONTROL = new Category(0x02, "MEDIA_CONTROL");
const Category* Category::WRITE_SETTING = new Category(0x03, "WRITE_SETTING");
