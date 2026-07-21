#include "CarduinoNodeCanSettings.h"

CarduinoNodeCanSettings::CarduinoNodeCanSettings() : CarduinoNodeExecutorInterface(EventCategory::SETTINGS) {};

void CarduinoNodeCanSettings::execute(CarduinoNode *node, Message *message) {
    this->node = node;

    switch(message->event->id) {
        case EV_READ_SETTING:
            readSetting(message);
            break;
        case EV_GET_SETTINGS:
            getSettings();
            break;
        case EV_WRITE_SETTING:
            writeSetting(message);
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

void CarduinoNodeCanSettings::writeSetting(Message *m) {
    auto *ev = static_cast<EventMulti<uint8_t, float>*>(m->event);
    uint8_t settingId = std::get<0>(ev->values);
    float raw = std::get<1>(ev->values); // I 4 byte arrivano "travestiti" da float

    auto it = node->settings.find(settingId);
    if (it == node->settings.end()) return;

    it->second->visit(
        [&](const Setting* s, int) { 
            // Reinterpretiamo i bit di 'raw' come se fossero un int
            int asInt;
            std::memcpy(&asInt, &raw, sizeof(float));
            
            // std::cout << "int " << asInt << std::endl;
            node->putSettingValue<int>(s, asInt); 
        },
        [&](const Setting* s, float) { 
            // Se la impostazione è davvero un float, il valore è già corretto
            // std::cout << "float " << raw << std::endl;
            node->putSettingValue<float>(s, raw); 
        },
        [&](const Setting* s, bool) { 
            // Anche per il bool, conviene estrarre prima l'intero originale
            int asInt;
            std::memcpy(&asInt, &raw, sizeof(float));
            bool asBool = (asInt != 0);
            
            // std::cout << "bool " << asBool << std::endl;
            node->putSettingValue<bool>(s, asBool); 
        }
    );
}
