#pragma once

#include <tuple>
#include <utility>
#include <string.h>
#include <stdlib.h>
#include <ostream>
#include <cmath>

#include "EventBase.h"

// ---------------------------------------------------------------------------
// Helper: parse a single token into the correct C++ type
// (namespace rinominato per evitare collisioni con SDK Arduino/ESP-IDF)
// ---------------------------------------------------------------------------
namespace canbus_detail {
    inline void parseToken(float&    out, const char* s) { out = (float)atof(s); }
    inline void parseToken(double&   out, const char* s) { out = atof(s); }
    inline void parseToken(uint8_t&  out, const char* s) { out = (uint8_t)atoi(s); }
    inline void parseToken(uint16_t& out, const char* s) { out = (uint16_t)atoi(s); }
    inline void parseToken(uint32_t& out, const char* s) { out = (uint32_t)strtoul(s, nullptr, 10); }
    inline void parseToken(int8_t&   out, const char* s) { out = (int8_t)atoi(s); }
    inline void parseToken(int16_t&  out, const char* s) { out = (int16_t)atoi(s); }
    inline void parseToken(int32_t&  out, const char* s) { out = (int32_t)atol(s); }
    inline void parseToken(bool&     out, const char* s) { out = (atoi(s) != 0); }
}

template<typename T>
static void printOne(std::ostream& out, const T& v) {
    if constexpr (std::is_same_v<T, uint8_t> || std::is_same_v<T, int8_t>) {
        out << static_cast<int>(v);
    } else {
        out << v;
    }
}

template <typename T>
static inline T clampFromFloat(float v) {
    if constexpr (std::is_same_v<T, bool>) {
        return v == 1.0f;
    } else if constexpr (std::is_integral_v<T>) {
        if (std::isnan(v)) return T{0};
        float r = std::round(v);
        r = std::max<float>(r, (float)std::numeric_limits<T>::min());
        r = std::min<float>(r, (float)std::numeric_limits<T>::max());
        return static_cast<T>(r);
    } else {
        return static_cast<T>(v); // float/double
    }
}


// ---------------------------------------------------------------------------
// EventMulti<Types...>
//   - Types = {} → evento senza payload (HEARTBEAT, ENABLE, ...)
//   - Types = {float} → evento con un float
//   - Types = {uint8_t, uint8_t, int32_t} → evento multi-valore
// ---------------------------------------------------------------------------
template<typename... Types>
class EventMulti : public EventBase {
public:
    std::tuple<Types...> values;

    EventMulti(uint8_t id, const char* name)
        : EventMulti(id, name, EventCategory::CONTROL) {}

    EventMulti(uint8_t id, const char* name, EventCategory category)
        : EventBase(id, name, category), values{} {}


    // --- Metadati -----------------------------------------------------------

    uint8_t valueCount() const override {
        return static_cast<uint8_t>(sizeof...(Types));
    }

    uint8_t payloadSize() const override {
        return static_cast<uint8_t>((sizeof(Types) + ... + 0));
    }

    // --- Serializzazione binaria --------------------------------------------

    void serialize(uint8_t* buf) const override {
        serializeImpl(buf, std::index_sequence_for<Types...>{});
    }

    void deserialize(const uint8_t* buf) override {
        deserializeImpl(buf, std::index_sequence_for<Types...>{});
    }

    // --- Parsing da token stringa -------------------------------------------

    bool deserializeFromTokens(const char** tokens, uint8_t count) override {
        if (count < sizeof...(Types)) return false;
        parseImpl(tokens, std::index_sequence_for<Types...>{});
        return true;
    }

    // --- Debug --------------------------------------------------------------

    void printValue(std::ostream& out, bool serialMode = false) const override {
        if(!serialMode) out << '[';
        printImpl(out, serialMode, std::index_sequence_for<Types...>{});
        if(!serialMode) out << ']';
    }

    void setFromFloat(float v) override {
        if constexpr (sizeof...(Types) > 0) {
            using T0 = std::tuple_element_t<0, std::tuple<Types...>>;
            std::get<0>(values) = clampFromFloat<T0>(v);
        }
    }

   float getAsFloat() const override {
        if constexpr (sizeof...(Types) > 0) {
            return static_cast<float>(std::get<0>(values));
        } else {
            return 0.0f;
        }
    }

private:
    // Serializzazione
    template<size_t... I>
    void serializeImpl(uint8_t* buf, std::index_sequence<I...>) const {
        if constexpr (sizeof...(I) > 0) {
            size_t offset = 0;
            ((memcpy(buf + offset, &std::get<I>(values), sizeof(Types)),
              offset += sizeof(Types)), ...);
        }
    }

    // Deserializzazione binaria
    template<size_t... I>
    void deserializeImpl(const uint8_t* buf, std::index_sequence<I...>) {
        if constexpr (sizeof...(I) > 0) {
            size_t offset = 0;
            ((memcpy(&std::get<I>(values), buf + offset, sizeof(Types)),
              offset += sizeof(Types)), ...);
        }
    }

    // Parsing da stringhe
    template<size_t... I>
    void parseImpl(const char** tokens, std::index_sequence<I...>) {
        if constexpr (sizeof...(I) > 0) {
            (canbus_detail::parseToken(std::get<I>(values), tokens[I]), ...);
        }
    }

    // Stampa
    template<size_t... I>
    void printImpl(std::ostream& out, bool serialMode, std::index_sequence<I...>) const {
        if constexpr (sizeof...(I) > 0) {
            bool first = true;
            const char* separator = serialMode ? ";" : ", ";
            ((out << (first ? "" : separator),
              printOne(out, std::get<I>(values)),
              first = false), ...);
            if(serialMode) out << separator;
        }
    }

};

// ---------------------------------------------------------------------------
// Alias comuni
// ---------------------------------------------------------------------------
using EventEmpty   = EventMulti<>;
using EventFloat   = EventMulti<float>;
using EventUInt8   = EventMulti<uint8_t>;
using EventUInt16  = EventMulti<uint16_t>;
using EventUInt32  = EventMulti<uint32_t>;
using EventInt32   = EventMulti<int32_t>;
using EventBool    = EventMulti<bool>;