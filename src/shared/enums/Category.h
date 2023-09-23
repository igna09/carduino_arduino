#pragma once

#include "Enum.h"

class ExecutorInterface; //forward declaration, needed to avoid circular dependency
class Category : public Enum {
    public:
        static const Category CAR_STATUS;
        static const Category READ_SETTINGS;
        static const Category MEDIA_CONTROL;
        static const Category WRITE_SETTING;

        Category() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Category::values[i]->id == id) {
                    return Category::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return Category::index;
        }

        static const Enum** getValues() {
            return Category::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;

        Category(uint8_t id, const char *name) : Enum(id, name) {
            Category::values[Category::index] = this;
            Category::index++;
        };
};

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* Category::values [4] = { 0 };
inline uint8_t Category::index = 0;
inline const Category Category::CAR_STATUS = Category(0x00, "CAR_STATUS");
inline const Category Category::READ_SETTINGS = Category(0x01, "READ_SETTINGS");
inline const Category Category::MEDIA_CONTROL = Category(0x02, "MEDIA_CONTROL");
inline const Category Category::WRITE_SETTING = Category(0x03, "WRITE_SETTING");
