#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"

// Forward declaration: evita l'include circolare con MainNode.h (che include
// questo header per registrare l'executor in MainNode::MainNode). La
// definizione completa di MainNode::recordHello non serve qui: basta la
// dichiarazione, risolta a link-time.
class MainNode;

// Intercetta EV_HELLO su MAIN e aggiorna la mappa _knownNodes di MainNode
// (id -> timestamp ultimo HELLO, vedi MainNode.h). Eseguito in parallelo a
// BootExecutor (entrambi sono sottoscritti a EV_HELLO e vengono eseguiti
// dall'executor CAN per ogni messaggio in arrivo): questo si limita a
// tracciare la presenza del nodo, non risponde con ENABLE.
//
// execute() è dichiarato qui ma definito in HelloTrackerExecutor.cpp: la
// definizione completa di MainNode (necessaria per chiamare recordHello) non
// può essere visibile in questo header senza ricreare l'include circolare
// con MainNode.h, quindi resta nel .cpp dove MainNode.h è incluso per intero.
class HelloTrackerExecutor: public CarduinoNodeExecutorInterface {
public:
    HelloTrackerExecutor() : CarduinoNodeExecutorInterface({EV_HELLO}) {};

    void execute(CarduinoNode *node, Message *message);
};