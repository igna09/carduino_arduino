#include "CruiseExecutor.h"
#include "KlineNode.h"
#include <string>

// Funzione helper per rimuovere spazi e isolare solo i bit '0' e '1'
static std::string extractBits(const char* raw) {
    if (!raw) return "";
    std::string cleanBits = "";
    for (const char* p = raw; *p != '\0'; ++p) {
        if (*p == '0' || *p == '1') {
            cleanBits += *p;
        }
    }
    return cleanBits;
}

// --- ANALISI PEDALS ---
static const char* PEDAL_NAMES[] = {
    "Frizione",    // Bit 0
    "Frenata",     // Bit 1
    "Freno luci"   // Bit 2
};

void CruiseExecutor::execute(CarduinoNode *carduinoNode) {
    auto* klineNode = static_cast<KlineNode*>(carduinoNode);

    // 1. Legge i valori correnti come const char*
    const char* rawPedals = klineNode->getLastValue<const char*>(ValueToRead::PEDALS.id);
    const char* rawCruise = klineNode->getLastValue<const char*>(ValueToRead::CRUISE_BITS.id);

    // 2. Trimmaggio e pulizia: "1  0  0" diventa "100", "100011" rimane "100011"
    std::string pedalsBits = extractBits(rawPedals);
    std::string cruiseBits = extractBits(rawCruise);


    // --- ANALISI PEDALS ---
    std::string pedalsLogMessage = "";
    bool pedalsChanged = false;

    for (size_t i = 0; i < pedalsBits.length(); ++i) {
        char currentBit = pedalsBits[i];
        char lastBit = (i < lastPedalsBits.length()) ? lastPedalsBits[i] : '0';

        if (currentBit != lastBit) {
            if (!pedalsLogMessage.empty()) {
                pedalsLogMessage += ", ";
            }
            
            // Assegna il nome specifico se rientra nei primi 3 bit, altrimenti usa il numero di bit
            std::string pedalName = (i < 3) ? PEDAL_NAMES[i] : ("Bit " + std::to_string(i));
            
            // Formattazione specifica per il pedale
            std::string statusBefore = (lastBit == '1') ? "ON" : "OFF";
            std::string statusAfter  = (currentBit == '1') ? "ON" : "OFF";
            pedalsLogMessage += pedalName + ": " + statusBefore + " -> " + statusAfter;
            pedalsChanged = true;
        }
    }

    // Unico log per i pedali
    if (pedalsChanged) {
        NLOGI("Pedals changes detected -> %s", pedalsLogMessage.c_str());
        lastPedalsBits = pedalsBits;
    }


    // --- ANALISI CRUISE ---
    std::string cruiseLogMessage = "";
    bool cruiseChanged = false;

    for (size_t i = 0; i < cruiseBits.length(); ++i) {
        char currentBit = cruiseBits[i];
        char lastBit = (i < lastCruiseBits.length()) ? lastCruiseBits[i] : '0';

        if (currentBit != lastBit) {
            if (!cruiseLogMessage.empty()) {
                cruiseLogMessage += ", ";
            }
            cruiseLogMessage += "Bit " + std::to_string(i) + ": " + lastBit + " -> " + currentBit;
            cruiseChanged = true;
        }
    }

    if (cruiseChanged) {
        NLOGI("Cruise changes detected -> %s", cruiseLogMessage.c_str());
        lastCruiseBits = cruiseBits; // Salva la nuova stringa pulita
    }
}