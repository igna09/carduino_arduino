#include "MainNodeSensorCanEvent.h"

MainNodeCanControlEvent::MainNodeCanControlEvent() : CarduinoNodeExecutorInterface(EventCategory::CONTROL) {};

void MainNodeCanControlEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    main->sendSerialMessage(*message);
};
