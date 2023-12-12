#pragma once

#include <Arduino.h>

class CategoryExecutorInterface {
    public:
        virtual void execute(String category, String key, String value) = 0;
};