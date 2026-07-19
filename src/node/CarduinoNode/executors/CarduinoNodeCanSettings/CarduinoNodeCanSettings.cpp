#include "CarduinoNodeCanSettings.h"

CarduinoNodeCanSettings::CarduinoNodeCanSettings() : CarduinoNodeExecutorInterface(EventCategory::SETTINGS) {};

void CarduinoNodeCanSettings::execute(CarduinoNode *node, Message *message) {
    this->node = node;

    switch(message->event->id) {
        case EV_READ_SETTING:
            // readSetting(message);
            break;
        case EV_GET_SETTINGS:
            getSettings();
            break;
    }
};

void CarduinoNodeCanSettings::readSetting(Message *message) {
    auto *ev = static_cast<EventMulti<uint8_t, float>*>(message->event);
    uint8_t settingId = std::get<0>(ev->values);

    if(!node->settings.contains(settingId)) {
        node->sendSerialMessage(*message);
    }
}

void CarduinoNodeCanSettings::getSettings() {
    for (auto &kv : node->settings) {
        Message m;
        kv.second->visit(
            [&](const Setting* s, int v)   { buildSettingMessage<int, int32_t>(node, s, &m, v); },
            [&](const Setting* s, float v) { buildSettingMessage<float>(node, s, &m, v); },
            [&](const Setting* s, bool v)  { buildSettingMessage<bool>(node, s, &m, v); }
        );
        node->sendMessage(m);
    }
}
