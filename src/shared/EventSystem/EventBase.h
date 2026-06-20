#pragma once

#include <cstdint>
#include <ostream>

class EventBase {
public:
    const uint8_t id;
    const char*   name;

    EventBase(uint8_t id, const char* name) : id(id), name(name) {}
    virtual ~EventBase() {}

    // Numero di valori nel payload
    virtual uint8_t valueCount() const = 0;

    // Dimensione totale in byte del payload
    virtual uint8_t payloadSize() const = 0;

    // Serializza i valori in un buffer binario
    virtual void serialize(uint8_t* buf) const = 0;

    // Deserializza da buffer binario
    virtual void deserialize(const uint8_t* buf) = 0;

    // Deserializza da array di stringhe (parsing da testo)
    virtual bool deserializeFromTokens(const char** tokens, uint8_t count) = 0;

    // Stampa i valori su Serial (debug)
    virtual void printValue(std::ostream& out, bool serialMode = false) const = 0;
};