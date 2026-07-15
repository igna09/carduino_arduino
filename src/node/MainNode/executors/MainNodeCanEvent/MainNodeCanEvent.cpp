#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void MainNodeCanEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    if(message->event->category == EventCategory::SENSOR ) {
        main->sendSerialMessage(*message);

        if(message->event->id == EV_SPEED) {
            auto* speedEv = static_cast<EventMulti<uint8_t>*>(message->event);
            uint8_t speed = std::get<0>(speedEv->values);
            main->lastSpeed = speed;
            main->_speedWarn.onSpeedUpdate(speed);
        }
    }
};

bool MainNodeCanEvent::canExecute(CarduinoNode *node, Message *message) {
    return true;
}
