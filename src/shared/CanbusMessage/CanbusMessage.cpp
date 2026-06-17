#include "CanbusMessage.h"

CanbusMessage::CanbusMessage() : eventId(0), targetNode(NODE_BROADCAST), sourceNode(0), priority(1), isExtended(false), payloadLength(0) {
    memset(payload, 0, sizeof(payload));
}

// Genera l'ID CAN grezzo in base allo standard scelto
uint32_t CanbusMessage::getCanId() const {
    if (isExtended) {
        // 29 bit: Prio(3) | Target(8) | Source(8) | Event(10)
        return ((uint32_t)(priority & 0x07) << 26) |
                ((uint32_t)targetNode << 18) |
                ((uint32_t)sourceNode << 10) |
                (eventId & 0x03FF);
    } else {
        // 11 bit: Prio(1) | Target(4) | Event(6)
        return ((uint32_t)(priority & 0x01) << 10) |
                ((uint32_t)(targetNode & 0x0F) << 6) |
                (eventId & 0x3F);
    }
}

// Parsa un ID CAN in ingresso popolando i campi dell'oggetto
void CanbusMessage::parseCanId(uint32_t canId, bool extended) {
    isExtended = extended;
    if (extended) {
        priority   = (canId >> 26) & 0x07;
        targetNode = (canId >> 18) & 0xFF;
        sourceNode = (canId >> 10) & 0xFF;
        eventId    = canId & 0x03FF;
    } else {
        priority   = (canId >> 10) & 0x01;
        targetNode = (canId >> 6) & 0x0F;
        sourceNode = 0; // Non presente nell'11 bit standard custom
        eventId    = canId & 0x3F;
    }
}

// ==========================================
// SISTEMA DI PACKING (Scrittura nel Payload)
// ==========================================
void CanbusMessage::resetPayload() {
    payloadLength = 0;
    memset(payload, 0, sizeof(payload));
}

String CanbusMessage::toSerialHumanString() {
    EventEnum* event = (EventEnum*)EventEnum::getValueById(this->eventId);

    const uint8_t* payload = this->payload;
    uint8_t payloadLength = this->payloadLength;
    
    String s = "";

    s += this->priority == 0 ? "HIGH" : "LOW";
    s += SERIAL_SEPARATOR;
    s += this->targetNode;
    s += SERIAL_SEPARATOR;
    s += event != nullptr ? event->name : String("UNKNOWN_EVENT_") + this->eventId;
    s += SERIAL_SEPARATOR;

    if(payloadLength > 0) {
        // s += "[";
        uint8_t offset = 0;
        bool hasAddedValue = false;

        for (uint8_t i = 0; i < MAX_VALUES_PER_PAYLOAD; i++) {
            DataTypeEnum type = event->types[i];
            
            // Condizione di uscita: fine dei tipi definiti o superamento del limite del pacchetto
            if (type.id == DataTypeEnum::NONE.id || offset >= payloadLength) {
                break; 
            }

            // if (hasAddedValue) {
            //     s += " "; // Separatore tra i valori (puoi usare anche la virgola se preferisci, es: ", ")
            // }

            // Estrazione programmatica basata sul tipo corrente e sull'offset calcolato
            if(type.id == DataTypeEnum::BOOL.id) {
                s += (payload[offset] != 0) ? "TRUE" : "FALSE";
            } else if (type.id == DataTypeEnum::INT8.id) {
                s += String((int8_t)payload[offset]);
            } else if (type.id == DataTypeEnum::UINT8.id) {
                s += String(payload[offset]);
            } else if (type.id == DataTypeEnum::INT16.id) {
                int16_t val; 
                memcpy(&val, &payload[offset], 2);
                s += String(val);
            } else if (type.id == DataTypeEnum::UINT16.id) {
                uint16_t val; 
                memcpy(&val, &payload[offset], 2);
                s += String(val);
            } else if (type.id == DataTypeEnum::INT32.id) {
                int32_t val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val);
            } else if (type.id == DataTypeEnum::UINT32.id) {
                uint32_t val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val);
            } else if (type.id == DataTypeEnum::FLOAT.id) {
                float val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val, 2); // Formattazione standard a 2 cifre decimali
            }

            // Avanzamento dell'offset in byte basato sul tipo appena processato
            offset += type.size;
            hasAddedValue = true;

            s += SERIAL_SEPARATOR;
        }

        // Se l'evento non conteneva in realtà nessun tipo (es. un trigger puro tipo ENABLE)
        // if (!hasAddedValue) {
        //     s += "TRIGGER";
        // }

        // s += "]";
    }
    return s;
}

String CanbusMessage::toSerialString() {
    EventEnum* event = (EventEnum*)EventEnum::getValueById(this->eventId);

    const uint8_t* payload = this->payload;
    uint8_t payloadLength = this->payloadLength;
    
    String s = "";

    s += this->priority;
    s += SERIAL_SEPARATOR;
    s += this->targetNode;
    s += SERIAL_SEPARATOR;
    s += this->eventId;
    s += SERIAL_SEPARATOR;

    if(payloadLength > 0) {
        // s += "[";
        uint8_t offset = 0;
        bool hasAddedValue = false;

        for (uint8_t i = 0; i < MAX_VALUES_PER_PAYLOAD; i++) {
            DataTypeEnum type = event->types[i];
            
            // Condizione di uscita: fine dei tipi definiti o superamento del limite del pacchetto
            if (type.id == DataTypeEnum::NONE.id || offset >= payloadLength) {
                break; 
            }

            // if (hasAddedValue) {
            //     s += " "; // Separatore tra i valori (puoi usare anche la virgola se preferisci, es: ", ")
            // }

            // Estrazione programmatica basata sul tipo corrente e sull'offset calcolato
            if(type.id == DataTypeEnum::BOOL.id) {
                s += (payload[offset] != 0) ? "TRUE" : "FALSE";
            } else if (type.id == DataTypeEnum::INT8.id) {
                s += String((int8_t)payload[offset]);
            } else if (type.id == DataTypeEnum::UINT8.id) {
                s += String(payload[offset]);
            } else if (type.id == DataTypeEnum::INT16.id) {
                int16_t val; 
                memcpy(&val, &payload[offset], 2);
                s += String(val);
            } else if (type.id == DataTypeEnum::UINT16.id) {
                uint16_t val; 
                memcpy(&val, &payload[offset], 2);
                s += String(val);
            } else if (type.id == DataTypeEnum::INT32.id) {
                int32_t val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val);
            } else if (type.id == DataTypeEnum::UINT32.id) {
                uint32_t val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val);
            } else if (type.id == DataTypeEnum::FLOAT.id) {
                float val; 
                memcpy(&val, &payload[offset], 4);
                s += String(val, 2); // Formattazione standard a 2 cifre decimali
            }

            // Avanzamento dell'offset in byte basato sul tipo appena processato
            offset += type.size;
            hasAddedValue = true;

            s += SERIAL_SEPARATOR;
        }

        // Se l'evento non conteneva in realtà nessun tipo (es. un trigger puro tipo ENABLE)
        // if (!hasAddedValue) {
        //     s += "TRIGGER";
        // }

        // s += "]";
    }
    return s;
}
