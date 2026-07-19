#include "CruiseExecutor.h"
#include "KlineNode.h"


void CruiseExecutor::execute(CarduinoNode *carduinoNode) {
    auto* klineNode = static_cast<KlineNode*>(carduinoNode);

    // Legge i valori correnti da kline
    int pedalsBits = klineNode->getLastValue<int>(ValueToRead::PEDALS.id);
    int cruiseBits = klineNode->getLastValue<int>(ValueToRead::CRUISE_BITS.id);

    // --- ANALISI PEDALS (Ultimi 3 bit) ---
    std::string pedalsLogMessage = "";
    bool pedalsChanged = false;

    for (int i = 0; i < 3; ++i) {
        bool currentBit = (pedalsBits >> i) & 1;
        bool lastBit    = (lastPedalsBits >> i) & 1;

        if (currentBit != lastBit) {
            if (!pedalsLogMessage.empty()) {
                pedalsLogMessage += ", ";
            }
            pedalsLogMessage += "Bit " + std::to_string(i) + ": " + std::to_string(lastBit) + " -> " + std::to_string(currentBit);
            pedalsChanged = true;
        }
    }

    // Unico log per i pedali se c'è stato almeno un cambiamento
    if (pedalsChanged) {
        NLOGI("Pedals changes detected -> %s", pedalsLogMessage.c_str());
        lastPedalsBits = pedalsBits;
    }


    // --- ANALISI CRUISE (Ultimi 6 bit) ---
    std::string cruiseLogMessage = "";
    bool cruiseChanged = false;

    for (int i = 0; i < 6; ++i) {
        bool currentBit = (cruiseBits >> i) & 1;
        bool lastBit    = (lastCruiseBits >> i) & 1;

        if (currentBit != lastBit) {
            if (!cruiseLogMessage.empty()) {
                cruiseLogMessage += ", ";
            }
            cruiseLogMessage += "Bit " + std::to_string(i) + ": " + std::to_string(lastBit) + " -> " + std::to_string(currentBit);
            cruiseChanged = true;
        }
    }

    // Unico log per il cruise se c'è stato almeno un cambiamento
    if (cruiseChanged) {
        NLOGI("Cruise changes detected -> %s", cruiseLogMessage.c_str());
        lastCruiseBits = cruiseBits;
    }
}