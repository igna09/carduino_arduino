#pragma once

#include <cstring>

#include "Enum.h"
#include "Definitions.h"

// Modifica questo numero in base a quanti nodi hai
#define NODE_SIZE 4

class Node : public Enum {
    public:
        // 1. Definisci i nodi qui (sostituiscono i vecchi #define)
        static const Node BROADCAST;
        static const Node MAIN;
        static const Node KLINE;
        static const Node TEST;

        gpio_num_t can_rx_pin;
        gpio_num_t can_tx_pin;

        Node() : Enum() {};

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Node::values[i]->id == id) {
                    return Node::values[i];
                }
            }
            return nullptr;
        }

        static const Enum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Node::values[i]->name, n) == 0) {
                    return Node::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() { return Node::index; }
        static const Enum** getValues() { return Node::values; }

    private:
        Node(uint8_t id, const char *name) : Enum(id, name) {
            Node::values[Node::index] = this;
            Node::index++;

            this->can_rx_pin = DEFAULT_CAN_RX_PIN;
            this->can_tx_pin = DEFAULT_CAN_TX_PIN;
        };
        Node(uint8_t id, const char *name, gpio_num_t rx, gpio_num_t tx) : Enum(id, name) {
            Node::values[Node::index] = this;
            Node::index++;

            this->can_rx_pin = rx;
            this->can_tx_pin = tx;
        };

        static const Enum* values[];
        static uint8_t index;
};

// 2. Inizializzazione unica dei valori e degli ID numerici
inline const Enum* Node::values[NODE_SIZE] = { 0 };
inline uint8_t Node::index = 0;

inline const Node Node::BROADCAST = Node(0x00, "BROADCAST");
inline const Node Node::MAIN = Node(0x01, "MAIN");
inline const Node Node::KLINE = Node(0x02, "KLINE");
inline const Node Node::TEST = Node(0x0F, "TEST");