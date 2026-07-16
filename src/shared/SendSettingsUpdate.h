#pragma once

#include "CarduinoNode.h"
#include "Setting.h"
#include "Message.h"

template<typename T, typename WireT = T>
static void buildSettingMessage(CarduinoNode *node, const Setting *setting, Message *message, T value) {
    auto* ev = static_cast<EventMulti<uint8_t, WireT>*>(EventRegistry::createById(EV_READ_SETTING));
    std::get<0>(ev->values) = setting->id;
    std::get<1>(ev->values) = static_cast<WireT>(value);

    message->priority = Priority::L.id;
    message->destination = Node::MAIN.id;
    message->event = ev;
}