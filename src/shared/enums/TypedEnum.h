#pragma once

#include "Enum.h"

class CanbusMessageType;
class TypedEnum : public Enum {
    public:
        const CanbusMessageType *type;

        TypedEnum() : Enum() {};
        TypedEnum(uint8_t id, const char *name, const CanbusMessageType *type) : Enum(id, name) {
            this->type = type;
        };
};
