#pragma once

#include "CarduinoNode.h"
#include "Setting.h"
#include "Message.h"

template<typename T, typename WireT = T>
static void buildSettingMessage(CarduinoNode *node, const Setting *setting, Message *message, T value) {
    auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(EventRegistry::createById(EV_READ_SETTING));
    std::get<0>(ev->values) = setting->id;

    int32_t wire;
    if constexpr (std::is_same_v<T, float>) {
        // decidi qui la strategia: bit-reinterpret o cast numerico
        std::memcpy(&wire, &value, sizeof(float)); // se serve bit-pattern
        // oppure: wire = static_cast<int32_t>(value); // se serve valore numerico
    } else {
        wire = static_cast<int32_t>(value);
    }
    std::get<1>(ev->values) = wire;

    message->priority = Priority::L.id;
    message->destination = Node::MAIN.id;
    message->event = ev;
}