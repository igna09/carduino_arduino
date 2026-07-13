#include "MainNodeCanEvent.h"

MainNodeCanEvent::MainNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void MainNodeCanEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    if(message->event->category == EventCategory::SENSOR ) {
        main->sendSerialMessage(*message);
    }
};

bool MainNodeCanEvent::canExecute(CarduinoNode *node, Message *message) {
    return true;
}
