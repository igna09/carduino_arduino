#pragma once

#include <cstdint>
#include <vector>
#include <initializer_list>

#include "Message.h"
#include "EventCategory.h"

class CarduinoNode;

class CarduinoNodeExecutorInterface {
    public:
        std::vector<uint8_t> eventIds;

        // Filtro per categoria (alternativo a eventIds). Se hasCategory è
        // true, matchesEvent ignora eventIds e confronta solo la categoria
        // dell'evento (via EventRegistry::getCategory), indipendentemente
        // dal fatto che eventIds sia vuoto o meno.
        EventCategory category = EventCategory::CONTROL;
        bool hasCategory = false;

        // Nessun filtro: valutato per ogni evento.
        CarduinoNodeExecutorInterface();

        // Filtro su singolo evento (comportamento storico, retrocompatibile
        // con CarduinoNodeSerialWriteSetting e simili).
        explicit CarduinoNodeExecutorInterface(uint8_t eventId);

        // Filtro su più eventi, es:
        //   CarduinoNodeExecutorInterface({EV_ENABLE, EV_DISABLE})
        CarduinoNodeExecutorInterface(std::initializer_list<uint8_t> eventIds);

        // Filtro su categoria, es:
        //   CarduinoNodeExecutorInterface(EventCategory::SENSOR)
        explicit CarduinoNodeExecutorInterface(EventCategory category);

        virtual ~CarduinoNodeExecutorInterface() = default;

        // True se l'executor non ha filtro, se ha un filtro di categoria che
        // combacia, oppure se eventId è tra quelli sorvegliati in eventIds.
        bool matchesEvent(uint8_t eventId) const;

        virtual void execute(CarduinoNode *node, Message *message) = 0;
        virtual bool canExecute(CarduinoNode *node, Message *message);
};