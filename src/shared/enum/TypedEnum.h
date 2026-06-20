#pragma once

#include "Enum.h"

class TypedEnum : public Enum {
    public:
        const MessageType *type;

        TypedEnum() : Enum() {};
        TypedEnum(uint8_t id, const char *name, const MessageType *type) : Enum(id, name) {
            this->type = type;
        };
};
