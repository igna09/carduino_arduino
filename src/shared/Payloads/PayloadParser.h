#pragma once

#include <Arduino.h>

class PayloadParser {
public:
    /**
     * @brief Converte un valore generico isolato dal payload in formato decimale stringa.
     * * @tparam T Il tipo di dato da estrarre (int, float, uint16_t, ecc.)
     * @param payload Il puntatore all'array di byte del payload
     * @param offset La posizione di partenza all'interno del payload (0-7)
     * @param decimals Il numero di cifre decimali (valido solo se T è float o double)
     * @return String La rappresentazione decimale leggibile
     */
    template<typename T>
    static String toDecimalString(const uint8_t* payload, uint8_t offset, uint8_t decimals = 2) {
        T value;
        // Copia in sicurezza i byte dal payload alla variabile temporanea
        memcpy(&value, &payload[offset], sizeof(T));
        
        // Se il tipo richiesto è un float o un double, usa la formattazione con decimali di Arduino
        if (is_floating_point<T>::value) {
            return String((double)value, decimals);
        }
        
        // Altrimenti gestisci come intero decimale standard
        return String(value);
    }

private:
    // Helper interni per verificare a tempo di compilazione se il tipo è un float/double
    template<typename T> struct is_floating_point { static const bool value = false; };
    template<> struct is_floating_point<float> { static const bool value = true; };
    template<> struct is_floating_point<double> { static const bool value = true; };
};