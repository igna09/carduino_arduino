#include "MainNodeCanSettings.h"

MainNodeCanSettings::MainNodeCanSettings() : CarduinoNodeExecutorInterface(EventCategory::SETTINGS) {};

void MainNodeCanSettings::execute(CarduinoNode *node, Message *message) {
    this->node = node;

    switch(message->event->id) {
        case EV_READ_SETTING:
            readSetting(message);
            break;
    }
};

void MainNodeCanSettings::readSetting(Message *message) {
    auto *ev = static_cast<EventMulti<uint8_t, float>*>(message->event);
    uint8_t settingId = std::get<0>(ev->values);

    if(!node->settings.contains(settingId)) {
        node->sendSerialMessage(*message);
    }
}
