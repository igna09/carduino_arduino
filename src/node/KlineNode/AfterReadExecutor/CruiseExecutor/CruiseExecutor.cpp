#include "CruiseExecutor.h"
#include "KlineNode.h"
#include <string>

// Funzione helper per rimuovere spazi e isolare solo i bit '0' e '1'
static std::string extractBits(const std::string& raw) {
    std::string cleanBits;
    cleanBits.reserve(raw.size()); // Pre-alloca la memoria per evitare riallocazioni

    for (char c : raw) {
        if (c == '0' || c == '1') {
            cleanBits += c;
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

    // Legge i valori correnti
    // const std::string rawPedals = klineNode->getLastValue<std::string>(ValueToRead::PEDALS.id);
    const std::string rawCruise = klineNode->getLastValue<std::string>(ValueToRead::CRUISE_BITS.id);
    const float cruiseSystem = klineNode->getLastValue<float>(ValueToRead::CRUISE_SYSTEM.id);

    // Trimmaggio e pulizia: "1  0  0" diventa "100", "100011" rimane "100011"
    // std::string pedalsBits = extractBits(rawPedals);
    std::string cruiseBits = extractBits(rawCruise);

    // Gestione PRIMO GIRO: Inizializza i valori senza loggare falsi cambiamenti
    /*if (lastPedalsBits.empty()) {
        lastPedalsBits = pedalsBits;
    }*/
    if (lastCruiseBits.empty()) {
        lastCruiseBits = cruiseBits;
    }

    /*if(pedalsBits != lastPedalsBits) {
        // --- ANALISI PEDALS ---
        std::string pedalsLogMessage = "";

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
            }
        }

        if (!pedalsLogMessage.empty()) {
            NLOGI("Pedals (%s) changes detected -> %s", pedalsBits.c_str(), pedalsLogMessage.c_str());
        }

        lastPedalsBits = pedalsBits;
    }*/

    if(cruiseBits != lastCruiseBits) {
        // --- ANALISI CRUISE ---
        std::string cruiseLogMessage = "";

        for (size_t i = 0; i < cruiseBits.length(); ++i) {
            char currentBit = cruiseBits[i];
            char lastBit = (i < lastCruiseBits.length()) ? lastCruiseBits[i] : '0';

            if (currentBit != lastBit) {
                if (!cruiseLogMessage.empty()) {
                    cruiseLogMessage += ", ";
                }
                cruiseLogMessage += "Bit " + std::to_string(i) + ": " + lastBit + " -> " + currentBit;
            }
        }

        
        if (!cruiseLogMessage.empty()) {
            NLOGI("Cruise (%s) changes detected -> %s", cruiseBits.c_str(), cruiseLogMessage.c_str());
        }

        lastCruiseBits = cruiseBits; // Salva la nuova stringa pulita
    }

    if(cruiseSystem != lastCruiseSystem) {
        NLOGI("Cruise system change detected: %f -> %f", lastCruiseSystem, cruiseSystem);
        lastCruiseSystem = cruiseSystem;

        auto* ev = static_cast<EventMulti<float>*>(EventRegistry::createById(EV_CRUISE_STATUS));
        std::get<0>(ev->values) = cruiseSystem;
        carduinoNode->sendMessage(Message(Priority::L.id, Node::BROADCAST.id, ev));
    }
}