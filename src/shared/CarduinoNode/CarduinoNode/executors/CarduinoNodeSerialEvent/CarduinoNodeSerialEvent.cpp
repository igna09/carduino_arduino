#include "CarduinoNodeSerialEvent.h"

CarduinoNodeSerialEvent::CarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    Serial.println(message->messageId);
    if(message->messageId == Event::TEST.id) {
        node->test();
    } else if(message->messageId == Event::ENABLE.id) {
        node->enable();
    } else if(message->messageId == Event::DISABLE.id) {
        node->disable();
    }
};
