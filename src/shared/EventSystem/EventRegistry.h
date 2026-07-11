#pragma once

#include <string.h>
#include <iomanip>

#include "EventBase.h"
#include "EventMulti.h"
#include "EventCategory.h"

#define EVENT_REGISTRY_MAX_SIZE 64

// ---------------------------------------------------------------------------
// EventRegistry
//   Registro statico di tutti gli eventi. Popolato automaticamente
//   dagli EventRegistrar<T> prima del main().
// ---------------------------------------------------------------------------
class EventRegistry {
public:
    using CreatorFn = EventBase* (*)(uint8_t id, const char* name, EventCategory cat);

    struct Entry {
        const char* name;
        uint8_t     id;
        CreatorFn   creator;
        EventCategory category;
    };

    static bool registerEvent(const char* name, uint8_t id, CreatorFn fn, EventCategory cat) {
        if (count >= EVENT_REGISTRY_MAX_SIZE) return false;
        entries[count++] = { name, id, fn, cat };
        return true;
    }

    static EventBase* createByName(const char* name) {
        for (uint8_t i = 0; i < count; i++)
            if (strcmp(entries[i].name, name) == 0)
                return entries[i].creator(entries[i].id, entries[i].name, entries[i].category);
        return nullptr;
    }

    static EventBase* createById(uint8_t id) {
        for (uint8_t i = 0; i < count; i++)
            if (entries[i].id == id)
                return entries[i].creator(entries[i].id, entries[i].name, entries[i].category);
        return nullptr;
    }

    static uint8_t size() { return count; }

    // Ritorna quanti eventi appartengono a una categoria
    static uint8_t countByCategory(EventCategory cat) {
        uint8_t n = 0;
        for (uint8_t i = 0; i < count; i++)
            if (entries[i].category == cat) n++;
        return n;
    }

    // Itera solo gli eventi di una categoria (utile per dump/filtri)
    template<typename Fn>
    static void forEachInCategory(EventCategory cat, Fn&& fn) {
        for (uint8_t i = 0; i < count; i++)
            if (entries[i].category == cat) fn(entries[i]);
    }

    // Debug: stampa tutti gli eventi registrati
    static void dump(std::ostream& out) {
        for (uint8_t i = 0; i < count; i++) {
            out << "  [0x";
            out << std::hex
                << std::setw(2)
                << std::setfill('0')
                << +entries[i].id;
            out << "] ";
            out << entries[i].name;
            out << " (" << toString(entries[i].category) << ")" << std::endl;
        }
    }

private:
    static Entry   entries[EVENT_REGISTRY_MAX_SIZE];
    static uint8_t count;
};

inline EventRegistry::Entry EventRegistry::entries[EVENT_REGISTRY_MAX_SIZE] = {};
inline uint8_t              EventRegistry::count = 0;

// ---------------------------------------------------------------------------
// EventRegistrar<T>
//   Un'istanza statica di questo oggetto per ogni evento.
//   Il costruttore viene chiamato prima di main() e registra l'evento.
// ---------------------------------------------------------------------------
template<typename T>
struct EventRegistrar {
    EventRegistrar(const char* name, uint8_t id, EventCategory cat) {
        EventRegistry::registerEvent(name, id,
            [](uint8_t id, const char* name, EventCategory cat) -> EventBase* {
                return new T(id, name, cat);
            }, cat
        );
    }
};

// ---------------------------------------------------------------------------
// DECLARE_EVENT(Name, id, "NAME", Types...)
//
//   Uso:
//     DECLARE_EVENT(HEARTBEAT,            0x08, "HEARTBEAT")
//     DECLARE_EVENT(SPEED,                0x16, "SPEED",         uint8_t)
//     DECLARE_EVENT(WRITE_SETTING,        0x2C, "WRITE_SETTING", uint8_t, int32_t)
// ---------------------------------------------------------------------------
#define DECLARE_EVENT(VarName, eventId, eventName, category, ...)  \
    static EventRegistrar<EventMulti<__VA_ARGS__>>                 \
        _registrar_##VarName(eventName, eventId, category);        \
    constexpr uint8_t EV_##VarName = eventId;
    // ↑ genera anche EV_SPEED, EV_HEARTBEAT, EV_WRITE_SETTING...