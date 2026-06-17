#pragma once

#include <Arduino.h>
#include "../utils.h"
#include "shared/SharedDefinitions.h"
#include "shared/enums/EventEnum/DataTypeEnum.h"
#include "shared/enums/EventEnum/EventEnum.h"

class CanbusMessage {
    public:
        uint8_t eventId;
        uint8_t targetNode;
        uint8_t sourceNode; // not used in 11 bit standard custom, but present in 29 bit extended
        uint8_t priority;
        bool isExtended;

        uint8_t payload[8];
        uint8_t payloadLength;

        CanbusMessage();

        // Genera l'ID CAN grezzo in base allo standard scelto
        uint32_t getCanId() const;

        // Parsa un ID CAN in ingresso popolando i campi dell'oggetto
        void parseCanId(uint32_t canId, bool extended);

        // ==========================================
        // SISTEMA DI PACKING (Scrittura nel Payload)
        // ==========================================
        void resetPayload();

        template<typename T>
        bool packValue(T value) {
            if (payloadLength + sizeof(T) > 8) return false; // Buffer overflow
            memcpy(&payload[payloadLength], &value, sizeof(T));
            payloadLength += sizeof(T);
            return true;
        }

        template<typename... Args>
        bool pack(Args... args) {
            // Esegue packValue per ogni argomento da sinistra a destra.
            // L'operatore '&&' interrompe l'esecuzione (short-circuit) se uno di essi fallisce.
            return (packValue(args) && ...);
        }

        // ============================================
        // SISTEMA DI UNPACKING (Lettura dal Payload)
        // ============================================
        template<typename T>
        T unpackValue(uint8_t offset) const {
            T value;
            if (offset + sizeof(T) <= 8) {
                memcpy(&value, &payload[offset], sizeof(T));
            } else {
                memset(&value, 0, sizeof(T)); // Ritorna 0 in caso di fuori bound
            }
            return value;
        }

        // static CanbusMessage* createSpecializedCopy(CanbusMessage* message) {
        //     return new CanbusMessage(message->id, message->payload, message->payloadLength);
        // };

        /**
         * @brief Converte il messaggio CAN in una stringa leggibile per il serial monitor, includendo la categoria, l'ID del messaggio e i valori del payload in formato decimale.
         * @return String La rappresentazione umana leggibile del messaggio CAN, ad esempio "CAR_STATUS;ENGINE_TELEMETRY;RPM:3000 CoolantTemp:90.5 CheckEngineLight:ON"
         * Nota: Questa funzione è pensata principalmente per il debug e la visualizzazione su seriale. Per la comunicazione tra nodi, è consigliabile utilizzare il formato binario o un formato più compatto.
         */

        String toSerialHumanString();

        String toSerialString();
};
