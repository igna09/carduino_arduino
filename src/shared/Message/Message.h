#pragma once

#include <stdlib.h>
#include <ostream>
#include <sstream>
#include <string>

#include "EventSystem.h"
#include "Setting.h"
#include "MessageType.h"
#include "Node.h"
#include "Priority.h"
#include "NodeLog.h"

#define CANBUSM_MAX_TOKENS 16
#define CANBUSM_BUF_SIZE   128

// ---------------------------------------------------------------------------
// Layout ID CAN standard (11 bit)
//
//  bit 10       bit 9..6        bit 5..0
// ┌─────────┬──────────────────┬──────────────────┐
// │  prio   │   destination    │    event id      │
// │  1 bit  │     4 bit        │     6 bit        │
// └─────────┴──────────────────┴──────────────────┘
//
//  canId = (priority << 10) | (destination << 6) | (eventId & 0x3F)
//
// Limiti:
//   priority    : 0..1    (1 bit)
//   destination : 0..15   (4 bit)
//   event id    : 0..63   (6 bit)
// ---------------------------------------------------------------------------
namespace CanId {
    constexpr uint8_t  EVENT_BITS  = 6;
    constexpr uint8_t  DST_BITS    = 4;
    constexpr uint8_t  PRIO_BITS   = 1;

    constexpr uint16_t EVENT_MASK  = (1u << EVENT_BITS) - 1;   // 0x3F
    constexpr uint16_t DST_MASK    = (1u << DST_BITS)   - 1;   // 0x0F
    constexpr uint16_t PRIO_MASK   = (1u << PRIO_BITS)  - 1;   // 0x01

    constexpr uint8_t  DST_SHIFT   = EVENT_BITS;                // 6
    constexpr uint8_t  PRIO_SHIFT  = EVENT_BITS + DST_BITS;     // 10

    // Componi i tre campi in un ID CAN a 11 bit
    inline uint16_t encode(uint8_t priority, uint8_t destination, uint8_t eventId) {
        return ((uint16_t)(priority    & PRIO_MASK)  << PRIO_SHIFT)
             | ((uint16_t)(destination & DST_MASK)   << DST_SHIFT)
             |  (uint16_t)(eventId     & EVENT_MASK);
    }

    inline uint8_t  decodePriority   (uint16_t canId) { return (canId >> PRIO_SHIFT) & PRIO_MASK; }
    inline uint8_t  decodeDestination(uint16_t canId) { return (canId >> DST_SHIFT)  & DST_MASK;  }
    inline uint8_t  decodeEventId    (uint16_t canId) { return  canId                & EVENT_MASK; }
}

// ---------------------------------------------------------------------------
// CanSymbol — risoluzione simbolica per priority, destination e value token
//
//   priority / destination
//     resolve(str, table) → uint8_t
//     Token numerico → atoi(); token simbolico → lookup nella tabella.
//     Nessuna ambiguità: un token che inizia con cifra non viene mai cercato
//     nella tabella.
//
//   value token (payload)
//     resolveValueToken(str, outBuf, outLen) → const char*
//     Lascia i token numerici invariati (nessuna sostituzione).
//     Per i token non numerici prova in cascata:
//       1. TRUE / FALSE          → "1" / "0"
//       2. Setting::getValueByName   → id come stringa decimale
//       3. MessageType::getValueByName → id come stringa decimale
//       4. Node::getValueByName → id come stringa decimale
//       5. fallback              → stringa originale invariata
//     Il risultato è scritto in outBuf se è stata fatta una sostituzione,
//     altrimenti viene restituito direttamente il puntatore originale.
// ---------------------------------------------------------------------------
namespace CanSymbol {

    // struct Entry { const char* name; uint8_t value; };

    // Priorità
    // static const Entry PRIORITIES[] = {
    //     { "LOW_PRIORITY",  0 },
    //     { "HIGH_PRIORITY", 1 },
    //     { nullptr,         0 }  // sentinel
    // };

    // -----------------------------------------------------------------------
    // resolve — per priority e destination
    //   Token numerico (inizia con cifra o '-') → atoi(), nessuna lookup.
    //   Token simbolico → cerca nella tabella, fallback atoi().
    // -----------------------------------------------------------------------
    // inline uint8_t resolve(const char* str, const Entry* table) {
    //     if ((*str >= '0' && *str <= '9') || *str == '-') return (uint8_t)atoi(str);
    //     for (const Entry* e = table; e->name != nullptr; ++e)
    //         if (strcmp(str, e->name) == 0) return e->value;
    //     return (uint8_t)atoi(str);
    // }

    // -----------------------------------------------------------------------
    // u8toa — helper interno: scrive v in decimale in buf, ritorna buf.
    // -----------------------------------------------------------------------
    inline const char* u8toa(uint8_t v, char* buf, uint8_t len) {
        uint8_t n = 0;
        if (v == 0) { buf[n++] = '0'; }
        else {
            uint8_t start = 0;
            while (v > 0 && n < len - 1) { buf[n++] = '0' + (v % 10); v /= 10; }
            for (uint8_t a = start, b = n - 1; a < b; a++, b--) {
                char tmp = buf[a]; buf[a] = buf[b]; buf[b] = tmp;
            }
        }
        buf[n] = '\0';
        return buf;
    }

    // -----------------------------------------------------------------------
    // resolveValueToken — per i token del payload
    //   Token numerico → ritorna str invariato (nessuna sostituzione).
    //   Token simbolico → prova in cascata i registry esistenti.
    //   outBuf/outLen: buffer temporaneo dove scrivere l'id come stringa
    //                  se viene fatta una sostituzione.
    // -----------------------------------------------------------------------
    inline const char* resolveValueToken(const char* str,
                                         char* outBuf, uint8_t outLen) {
        // Token numerico (o negativo): passa as-is, zero ambiguità
        if ((*str >= '0' && *str <= '9') || *str == '-') return str;

        // TRUE / FALSE: costanti universali, non passano per nessun registry
        if (strcmp(str, "TRUE")  == 0) return "1";
        if (strcmp(str, "FALSE") == 0) return "0";

        // Setting (TypedEnum con name → id)
        {
            const Setting* s = (Setting*) Setting::getValueByName(const_cast<char*>(str));
            if (s) return u8toa(s->id, outBuf, outLen);
        }

        // MessageType (Enum con name → id)
        {
            const MessageType* t = (MessageType*) MessageType::getValueByName(const_cast<char*>(str));
            if (t) return u8toa(t->id, outBuf, outLen);
        }

        {
            const Node* n = (Node*) Node::getValueByName(const_cast<char*>(str));
            if (n) return u8toa(n->id, outBuf, outLen);
        }

        {
            const Priority* p = (Priority*) Priority::getValueByName(const_cast<char*>(str));
            if (p) return u8toa(p->id, outBuf, outLen);
        }

        // Simbolo non riconosciuto: passa as-is (l'evento deciderà)
        return str;
    }

} // namespace CanSymbol

// ---------------------------------------------------------------------------
// Message
//   Rappresenta un messaggio CAN completo.
//
//   Wire format:
//     [CAN ID 11bit] → packed nei bit come da CanId::encode()
//     [payload 0..8 byte] → serializzato dall'evento
//
//   String format:
//     "priorità;destinatario;NOME_EVENTO;val1;val2;...;"
//     es: "1;0;WRITE_SETTING;1;2;300;"
// ---------------------------------------------------------------------------
class Message {
public:
    uint8_t    priority;       // 0..1
    uint8_t    destination;    // 0..15
    EventBase* event;          // owning pointer

    Message() {}

    Message(uint8_t prio, uint8_t dst, EventBase* ev)
        : priority(prio), destination(dst), event(ev) {}

    ~Message() { delete event; }

    // Non copiabile
    Message(const Message&)            = delete;
    Message& operator=(const Message&) = delete;

    // Movibile
    Message(Message&& other) noexcept
        : priority(other.priority), destination(other.destination), event(other.event) {
        other.event = nullptr;
    }

    // -----------------------------------------------------------------------
    // canId
    //   Ritorna l'ID CAN a 11 bit pronto da passare al driver CAN.
    // -----------------------------------------------------------------------
    uint16_t canId() const {
        return CanId::encode(priority, destination, event->id);
    }

    // -----------------------------------------------------------------------
    // fromCanFrame
    //   Costruisce il messaggio dall'ID CAN a 11 bit + buffer payload.
    //   Da usare quando si riceve un frame dal bus CAN.
    // -----------------------------------------------------------------------
    static Message* fromCanFrame(uint16_t canId, const uint8_t* payload, uint8_t payloadLen) {
        uint8_t priority    = CanId::decodePriority(canId);
        uint8_t destination = CanId::decodeDestination(canId);
        uint8_t eventId     = CanId::decodeEventId(canId);

        EventBase* ev = EventRegistry::createById(eventId);
        if (!ev) return nullptr;

        if (payloadLen < ev->payloadSize()) {
            delete ev;
            return nullptr;
        }

        ev->deserialize(payload);
        return new Message(priority, destination, ev);
    }

    // -----------------------------------------------------------------------
    // toCanFrame
    //   Riempie canIdOut e payloadBuf pronti per la trasmissione sul bus.
    //   Ritorna la lunghezza del payload scritto (DLC).
    // -----------------------------------------------------------------------
    uint8_t toCanFrame(uint16_t& canIdOut, uint8_t* payloadBuf, uint8_t bufLen) const {
        if (bufLen < event->payloadSize()) return 0;
        canIdOut = canId();
        event->serialize(payloadBuf);
        return event->payloadSize();
    }

    // -----------------------------------------------------------------------
    // fromString
    //   Parsa la stringa e costruisce il messaggio.
    //   Ritorna nullptr se il formato è invalido o l'evento non è registrato.
    // -----------------------------------------------------------------------
    static Message* fromString(const char* input) {
        if (!input || !*input) return nullptr;

        static char buf[CANBUSM_BUF_SIZE];
        strncpy(buf, input, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        const char* tokens[CANBUSM_MAX_TOKENS];
        uint8_t tokenCount = 0;

        char* tok = strtok(buf, ";");
        while (tok && tokenCount < CANBUSM_MAX_TOKENS) {
            tokens[tokenCount++] = tok;
            tok = strtok(nullptr, ";");
        }

        if (tokenCount < 3) return nullptr;

        // Accetta sia il valore numerico ("0", "1") sia il token simbolico
        // ("L"/"H" per la priorità;
        //  "BROADCAST", "MAIN", … per la destinazione).
        // const char* priority    = tokens[0];
        // const char* destination = tokens[1];
        const char* eventName   = tokens[2];

        EventBase* ev = EventRegistry::createByName(eventName);
        if (!ev) return nullptr;

        // const char** valueTokens = tokens + 3;
        // uint8_t      valueCount  = tokenCount - 3;
        const char** valueTokens = tokens;
        uint8_t      valueCount  = tokenCount;

        // Pre-processa i value token: sostituisce simboli noti con la loro
        // rappresentazione numerica. I token già numerici passano invariati.
        // outBufs fornisce lo spazio temporaneo per le sostituzioni: ogni
        // slot è grande abbastanza per un uint8_t in decimale (max "255\0").
        char        outBufs[CANBUSM_MAX_TOKENS][4];
        const char* resolved[CANBUSM_MAX_TOKENS];
        for (uint8_t i = 0; i < valueCount; i++) {
            resolved[i] = CanSymbol::resolveValueToken(
                valueTokens[i], outBufs[i], sizeof(outBufs[i]));
        }

        if (!ev->deserializeFromTokens(resolved, valueCount)) {
            delete ev;
            return nullptr;
        }

        return new Message((uint8_t)atoi(resolved[0]), (uint8_t)atoi(resolved[1]), ev);
    }

    // -----------------------------------------------------------------------
    // print / toString
    // -----------------------------------------------------------------------

    // Stampa su qualsiasi Print& (Serial, SoftwareSerial, WiFiClient, ...)
    void print(std::ostream& out, bool serialMode = false) const {
        if (serialMode) {
            // out << (unsigned)priority;
            // out << ';';
            // out << (unsigned)destination;
            // out << ';';
            // out << eventCategoryToString(event->category);
            // out << ';';
            // out << event->name;
            out << (int)event->id;
            out << ';';
            event->printValue(out, true);
        } else {
            out << "{ canId=0x";
            out << std::hex
                << std::setw(2)
                << std::setfill('0')
                << canId()
                << std::dec;
            out << " [prio=";
            out << (unsigned)priority;
            out << ", dst=";
            out << (unsigned)destination;
            out << ", event=0x";
            out << std::hex
                << std::setw(2)
                << std::setfill('0')
                << (unsigned)event->id
                << std::dec;
            out << "] ";
            out << event->name;
            out << ' ';
            event->printValue(out, false);
            out << " }";
        }
        // out << std::endl << std::flush;
    }

    std::string toString(bool serialMode = false) const {
        std::ostringstream ss;
        print(ss, serialMode); // Passiamo il flusso alla tua nuova funzione printValue
        return ss.str();            // Ritorna una std::string pronta all'uso
    }
};