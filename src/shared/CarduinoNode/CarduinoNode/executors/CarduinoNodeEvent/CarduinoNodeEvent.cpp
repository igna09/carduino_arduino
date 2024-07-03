#include "CarduinoNodeEvent.h"

CarduinoNodeEvent::CarduinoNodeEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    Serial.println(message->messageId);
    if(message->messageId == Event::TEST.id) {
        node->test();
    } else if(message->messageId == Event::ENABLE.id) {
        node->enable();
    } else if(message->messageId == Event::DISABLE.id) {
        node->disable();
    }
};
