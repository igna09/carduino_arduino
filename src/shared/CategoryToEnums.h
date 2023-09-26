#pragma once

#include <Arduino.h>
#include <functional>
#include "enums/Category.h"
#include "enums/Carstatus.h"
#include "enums/Setting.h"

class CategoryToEnums {
    public:
        static const CategoryToEnums CARSTATUS;
        static const CategoryToEnums READ_SETTINGS;
        static const CategoryToEnums WRITE_SETTINGS;

        Category *category;
        std::function<Enum*(char*)> enumNameToCategoryFunction;

        static const CategoryToEnums* getValueByCategory(Category *c) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(CategoryToEnums::values[i]->category->id == c->id) {
                    return CategoryToEnums::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return CategoryToEnums::index;
        }

        static const CategoryToEnums** getValues() {
            return CategoryToEnums::values;
        }

    private:

    private:
        static const CategoryToEnums* values[];
        static uint8_t index;

        CategoryToEnums(const Category*, const Enum* (&)(char*)) {
            CategoryToEnums::values[CategoryToEnums::index] = this;
            CategoryToEnums::index++;
        };
};

inline const CategoryToEnums* CategoryToEnums::values [4] = { 0 };
inline uint8_t CategoryToEnums::index = 0;
inline const CategoryToEnums CategoryToEnums::CARSTATUS = CategoryToEnums(&Category::CAR_STATUS, Carstatus::getValueByName);
inline const CategoryToEnums CategoryToEnums::READ_SETTINGS = CategoryToEnums(&Category::READ_SETTINGS, Setting::getValueByName);
inline const CategoryToEnums CategoryToEnums::WRITE_SETTINGS = CategoryToEnums(&Category::WRITE_SETTING, Setting::getValueByName);
