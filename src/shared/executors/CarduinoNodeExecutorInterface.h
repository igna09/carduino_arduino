#pragma once

#include <cstdint>
#include <vector>
#include <initializer_list>

#include "Message.h"

// Forward declaration: CarduinoNode.h include Executor.h, che a sua volta
// userebbe questa interfaccia. Includere CarduinoNode.h per intero qui
// creerebbe un ciclo di dipendenza. Dato che CarduinoNode è usato solo come
// puntatore nelle firme sotto (mai dereferenziato/istanziato in questo
// header), la forward declaration basta: la definizione completa viene
// richiesta solo nei .cpp che implementano execute()/canExecute().
class CarduinoNode;

class CarduinoNodeExecutorInterface {
    public:
        // eventIds vuoto = nessun filtro: l'executor viene valutato per
        // ogni messaggio (equivalente al vecchio filterEvent == false).
        // eventIds non vuoto = l'executor viene valutato solo se
        // message->event->id è presente nella lista (il vecchio comportamento
        // a singolo id era il caso eventIds.size() == 1).
        std::vector<uint8_t> eventIds;

        // Nessun filtro: valutato per ogni evento.
        CarduinoNodeExecutorInterface();

        // Filtro su singolo evento (comportamento storico, retrocompatibile
        // con CarduinoNodeSerialWriteSetting e simili).
        explicit CarduinoNodeExecutorInterface(uint8_t eventId);

        // Filtro su più eventi, es:
        //   CarduinoNodeExecutorInterface({EV_ENABLE, EV_DISABLE})
        CarduinoNodeExecutorInterface(std::initializer_list<uint8_t> eventIds);

        virtual ~CarduinoNodeExecutorInterface() = default;

        // True se l'executor non ha filtro, oppure se eventId è tra quelli
        // sorvegliati. Centralizza qui la logica di match così Executor::execute
        // non deve conoscere la rappresentazione interna del filtro.
        bool matchesEvent(uint8_t eventId) const;

        // Message* è ricevuto in prestito: l'executor non ne diventa owner,
        // non deve mai fare delete né estendere la sua vita oltre la
        // chiamata (stessa convenzione di CarduinoNode::onMessageReceived,
        // ma lì il ricevente È owner; qui Executor passa solo un puntatore
        // a un Message di cui resta owner il chiamante originale in
        // rxTaskEntry).
        virtual void execute(CarduinoNode *node, Message *message) = 0;
        virtual bool canExecute(CarduinoNode *node, Message *message);
};