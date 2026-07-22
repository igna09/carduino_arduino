#pragma once

#include <cstring>

#include "Enum.h"

#define MESSAGE_TYPE_SIZE 5

class MessageType : public Enum {
    public:
        static const MessageType INT;
        static const MessageType FLOAT;
        static const MessageType TEXT;
        static const MessageType BOOL;
        static const MessageType BYTE;

        MessageType() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(MessageType::values[i]->id == id) {
                    return MessageType::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(MessageType::values[i]->name, n) == 0) {
                    return MessageType::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return MessageType::index;
        }

        static const Enum** getValues() {
            return MessageType::values;
        }

        MessageType(uint8_t id, const char *name) : Enum(id, name) {
            MessageType::values[MessageType::index] = this;
            MessageType::index++;
        };

    private:
        static const Enum* values[];
        static uint8_t index;
};

inline const Enum* MessageType::values [MESSAGE_TYPE_SIZE] = { 0 };
inline uint8_t MessageType::index = 0;
inline const MessageType MessageType::INT = MessageType(0x00, "INT");
inline const MessageType MessageType::FLOAT = MessageType(0x01, "FLOAT");
inline const MessageType MessageType::TEXT = MessageType(0x02, "TEXT");
inline const MessageType MessageType::BOOL = MessageType(0x03, "BOOL");
inline const MessageType MessageType::BYTE = MessageType(0x04, "BYTE");
