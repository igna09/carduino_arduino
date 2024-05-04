#pragma once

#include "Enum.h"
#include "Carstatus.h"
#include "Setting.h"
#include "MediaControl.h"
#include "Event.h"

#define CATEGORY_SIZE 7

class CanbusMessage;
class Category : public Enum {
    public:
        static const Category CAR_STATUS;
        static const Category READ_SETTING; // used to receive setting value
        static const Category MEDIA_CONTROL;
        static const Category WRITE_SETTING;
        // static const Category HEARTBEAT;
        // static const Category ERROR;
        static const Category EVENT;
        static const Category GET_SETTINGS; // used to start reading all settings


        std::function<CanbusMessage*(CanbusMessage*)> createSpecializedCopyFunction;
        std::function<const TypedEnum**()> getCategoryValuesFunction;
        std::function<const TypedEnum*(char*)> getEnumFromNameFunction;

        Category() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Category::values[i]->id == id) {
                    return Category::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Category::values[i]->name, n) == 0) {
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

        Category(uint8_t id, const char *name, std::function<const TypedEnum**()> getCategoryValuesFunction, std::function<CanbusMessage*(CanbusMessage*)> createSpecializedCopyFunction) : Enum(id, name) {
            Category::values[Category::index] = this;
            Category::index++;

            this->createSpecializedCopyFunction = createSpecializedCopyFunction;
            this->getCategoryValuesFunction = getCategoryValuesFunction;
            this->getEnumFromNameFunction = nullptr;
        };

        Category(uint8_t id, const char *name, std::function<const TypedEnum**()> getCategoryValuesFunction, std::function<CanbusMessage*(CanbusMessage*)> createSpecializedCopyFunction, std::function<const TypedEnum*(char*)> convertCallback) : Enum(id, name) {
            Category::values[Category::index] = this;
            Category::index++;

            this->createSpecializedCopyFunction = createSpecializedCopyFunction;
            this->getCategoryValuesFunction = getCategoryValuesFunction;
            this->getEnumFromNameFunction = convertCallback;
        };
};
