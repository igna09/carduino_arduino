#include "MainNodeSensorCanEvent.h"

MainNodeSensorCanEvent::MainNodeSensorCanEvent() : CarduinoNodeExecutorInterface(EventCategory::SENSOR) {};

void MainNodeSensorCanEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    main->sendSerialMessage(*message);

    if(message->event->id == EV_SPEED) {
        auto settingPtr = main->getSetting<bool>(&Setting::SPD_LMT_LRM);
        if (settingPtr != nullptr && !settingPtr->value) return;

        auto* speedEv = static_cast<EventMulti<uint8_t>*>(message->event);
        uint8_t speed = std::get<0>(speedEv->values);
        main->lastSpeed = speed;
        main->_speedWarn.onSpeedUpdate(speed);
    }
};
