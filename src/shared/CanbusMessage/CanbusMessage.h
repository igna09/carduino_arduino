#pragma once

#include <stdlib.h>
#include <string.h>

#include "shared/EventSystem/EventSystem.h"

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
// CanbusMessage
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
class CanbusMessage {
public:
    uint8_t    priority;       // 0..1
    uint8_t    destination;    // 0..15
    EventBase* event;          // owning pointer

    CanbusMessage(uint8_t prio, uint8_t dst, EventBase* ev)
        : priority(prio), destination(dst), event(ev) {}

    ~CanbusMessage() { delete event; }

    // Non copiabile
    CanbusMessage(const CanbusMessage&)            = delete;
    CanbusMessage& operator=(const CanbusMessage&) = delete;

    // Movibile
    CanbusMessage(CanbusMessage&& other) noexcept
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
    static CanbusMessage* fromCanFrame(uint16_t canId, const uint8_t* payload, uint8_t payloadLen) {
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
        return new CanbusMessage(priority, destination, ev);
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
    static CanbusMessage* fromString(const char* input) {
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

        uint8_t     priority    = (uint8_t)atoi(tokens[0]);
        uint8_t     destination = (uint8_t)atoi(tokens[1]);
        const char* eventName   = tokens[2];

        EventBase* ev = EventRegistry::createByName(eventName);
        if (!ev) return nullptr;

        const char** valueTokens = tokens + 3;
        uint8_t      valueCount  = tokenCount - 3;

        if (!ev->deserializeFromTokens(valueTokens, valueCount)) {
            delete ev;
            return nullptr;
        }

        return new CanbusMessage(priority, destination, ev);
    }

    // -----------------------------------------------------------------------
    // print / toString / toArduinoString
    // -----------------------------------------------------------------------

    // Stampa su qualsiasi Print& (Serial, SoftwareSerial, WiFiClient, ...)
    void print(Print& out, bool serialMode = false) const {
        if (serialMode) {
            out.print(priority);
            out.print(F(";"));
            out.print(destination);
            out.print(F(";"));
            out.print(event->id);
            out.print(F(";"));
            event->printValue(out, true);
        } else {
            out.print(F("CanbusMessage { canId=0x"));
            out.print(canId(), HEX);
            out.print(F(" [prio="));
            out.print(priority);
            out.print(F(", dst="));
            out.print(destination);
            out.print(F(", event=0x"));
            out.print(event->id, HEX);
            out.print(F("] "));
            out.print(event->name);
            out.print(F(" "));
            event->printValue(out, false);
            out.print(F(" }"));
        }
    }

    // Scrive in un char[] fornito dall'utente.
    // Ritorna il numero di caratteri scritti (senza il '\0' finale).
    uint8_t toString(char* buf, uint8_t bufLen, bool serialMode = false) const {
        PrintToBuf p(buf, bufLen);
        print(p, serialMode);
        return p.written();
    }

    // Ritorna uno String Arduino (alloca heap — usare solo dove accettabile).
    String toArduinoString(bool serialMode = false) const {
        char buf[128];
        toString(buf, sizeof(buf), serialMode);
        return String(buf);
    }

private:
    // Helper interno: adattatore Print → char[]
    // Estende Print e accumula i caratteri in un buffer esterno.
    class PrintToBuf : public Print {
    public:
        PrintToBuf(char* buf, uint8_t len)
            : _buf(buf), _len(len), _pos(0) {
            if (_len > 0) _buf[0] = '\0';
        }

        size_t write(uint8_t c) override {
            if (_pos < _len - 1) {
                _buf[_pos++] = (char)c;
                _buf[_pos]   = '\0';
            }
            return 1;
        }

        size_t write(const uint8_t* buf, size_t size) override {
            for (size_t i = 0; i < size; i++) write(buf[i]);
            return size;
        }

        uint8_t written() const { return _pos; }

    private:
        char*   _buf;
        uint8_t _len;
        uint8_t _pos;
    };
};