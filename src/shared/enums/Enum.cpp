#include "Enum.h"

Enum::Enum() {}

Enum::Enum(uint8_t id, char *name) {
    this->name = name;
    this->id = id;
};

bool Enum::operator== (const Enum &e)
{
   return id == e.id;
}
