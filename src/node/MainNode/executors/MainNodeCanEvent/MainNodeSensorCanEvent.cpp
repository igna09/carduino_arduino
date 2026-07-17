#include "MainNodeSensorCanEvent.h"

MainNodeSensorCanEvent::MainNodeSensorCanEvent() : CarduinoNodeExecutorInterface() {};

void MainNodeSensorCanEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    bool handleKline = true;

    auto settingPtr = main->getSetting<bool>(&Setting::HANDLE_KLINE);
    if (settingPtr != nullptr) {
        handleKline = settingPtr->value;
    }

    if(message->event->category == EventCategory::SENSOR && handleKline) {
        main->sendSerialMessage(*message);

        if(message->event->id == EV_SPEED) {
            auto* speedEv = static_cast<EventMulti<uint8_t>*>(message->event);
            uint8_t speed = std::get<0>(speedEv->values);
            main->lastSpeed = speed;
            main->_speedWarn.onSpeedUpdate(speed);
        }
    }
};
