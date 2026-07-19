#pragma once

#include <string> // Incluso per la gestione delle stringhe di log

#include "AfterReadExecutorInterface.h"
#include "ValueToRead.h"
#include "CarduinoNode.h"
#include "NodeLog.h"

class CruiseExecutor : public AfterReadExecutorInterface {
    public:
        void execute(CarduinoNode *carduinoNode) override; //[cite: 2]

    private:
        // Variabili di istanza come interi per salvare l'ultimo valore loggato
        int lastPedalsBits = 0;
        int lastCruiseBits = 0;
};