#include "CarduinoNodeCanGetSettings.h"

CarduinoNodeCanGetSettings::CarduinoNodeCanGetSettings() : CarduinoNodeExecutorInterface(EV_GET_SETTINGS) {};

void CarduinoNodeCanGetSettings::execute(CarduinoNode *node, Message *message) {
    for (auto &kv : node->settings) {
        Message m;
        kv.second->visit(
            [&](const Setting* s, int v)   { buildSettingMessage<int, int32_t>(node, s, &m, v); },
            [&](const Setting* s, float v) { buildSettingMessage<float>(node, s, &m, v); },
            [&](const Setting* s, bool v)  { buildSettingMessage<bool>(node, s, &m, v); }
        );
        node->sendMessage(m);
    }
};
