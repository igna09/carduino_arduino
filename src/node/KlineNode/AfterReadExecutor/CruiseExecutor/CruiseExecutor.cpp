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

// --- ENUM CRUISE STATE (max 6 caratteri) ---
enum class CruiseState : uint8_t {
    OFF    = 0,
    READY  = 1,
    ACTIVE = 2,
    BRAKE  = 3,
    CLUTCH = 4,
    PAUSED = 5,
    ERROR = 6
};

static CruiseState determineCruiseState(const std::string& cruiseBits, float cruiseSystem) {
    // 1. PRIMARIA VERIFICA PEDALI DAI BIT (Stringa "100011" -> Index 0 = Bit 5, Index 1 = Bit 4)
    if (cruiseBits.length() >= 6) {
        if(cruiseBits[5] == '1' && cruiseBits[4] == '0') {
            return CruiseState::ERROR;
        }
        // Bit 5 = Frizione (Fondamentale, dato che cruiseSystem non cambia quando premi la frizione)
        if (cruiseBits[0] == '1') {
            return CruiseState::CLUTCH;
        }
        // Bit 4 = Freno
        if (cruiseBits[1] == '1') {
            return CruiseState::BRAKE;
        }
    }

    // 2. VERIFICA STATO CENTRALINA MOTORE (cruiseSystem)
    int statusInt = static_cast<int>(cruiseSystem);
    switch (statusInt) {
        case 0:  return CruiseState::OFF;
        case 1:  return CruiseState::READY;
        case 9:  return CruiseState::ACTIVE;
        case 17: return CruiseState::BRAKE;  // Il valore 17 corrisponde al Freno lato ECU
        case 2:  return CruiseState::PAUSED;
        default: return (cruiseSystem > 0) ? CruiseState::READY : CruiseState::OFF;
    }
}

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
    }

    // Determina lo stato unificato con priorità alla frizione/freno
    CruiseState currentState = determineCruiseState(cruiseBits, cruiseSystem);

    // Invio ad Android
    auto* ev = static_cast<EventMulti<uint8_t>*>(EventRegistry::createById(EV_CRUISE_STATUS));
    std::get<0>(ev->values) = static_cast<uint8_t>(currentState);
    carduinoNode->sendMessage(Message(Priority::L.id, Node::BROADCAST.id, ev));
}