#pragma once

#include "Enum.h"

class ExecutorInterface; //forward declaration, needed to avoid circular dependency
class Category : public Enum {
    public:
        static const Category CAR_STATUS;
        static const Category READ_SETTINGS;
        static const Category MEDIA_CONTROL;
        static const Category WRITE_SETTING;

        static const Enum* getValueById(uint8_t id);
        static const Enum** getValues();
        static uint8_t getSize();

    private:
        static const Enum* values[];
        static uint8_t index;
        
        Category(uint8_t id, const char *name);
};
