#pragma once

#include <cstdint>
#include <vector>

#include "Message.h"
#include "CarduinoNodeExecutorInterface.h"
#include "NodeLog.h"

// Forward declaration, non include completo: CarduinoNode.h include questo
// header (possiede serialExecutor/canExecutor come campi), quindi includere
// "CarduinoNode.h" qui per intero richiuderebbe il ciclo che la versione
// Arduino aveva. Execute() usa CarduinoNode solo come puntatore passato a
// valle, mai dereferenziato qui: la forward declaration è sufficiente.
class CarduinoNode;

class Executor {
    public:
        // Sostituito l'array a dimensione fissa (EXECUTORS_SIZE) con
        // std::vector: niente più limite arbitrario né rischio di overflow
        // silenzioso su addExecutor oltre la capacità. ESP-IDF ha heap
        // disponibile (non siamo su AVR), e gli executor vengono registrati
        // una sola volta in fase di setup, quindi il costo di eventuali
        // realloc è irrilevante e pagato una tantum.
        std::vector<CarduinoNodeExecutorInterface*> executors;

        void addExecutor(CarduinoNodeExecutorInterface* e);
        void execute(CarduinoNode *node, Message *message);
};