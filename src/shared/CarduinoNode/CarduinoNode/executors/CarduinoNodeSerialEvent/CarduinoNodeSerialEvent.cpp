#include "CarduinoNodeSerialEvent.h"

CarduinoNodeSerialEvent::CarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);
    if(eventMessage->getIntValue() != node->id || eventMessage->getIntValue() == ALL_NODES) {
        node->sendCanbusMessage(eventMessage);
    }
    if(eventMessage->getIntValue() == node->id || eventMessage->getIntValue() == ALL_NODES) {
        if(message->messageId == Event::RESTART.id) {
            node->delayTask(1000, [&](){
                node->restart();
            });
        }
    }
    delete eventMessage;

    /**
     * testing purposes
     */
    if(message->messageId == Event::TEST.id) {
        node->test();
    } else if(message->messageId == Event::ENABLE.id) {
        node->enable();
    } else if(message->messageId == Event::DISABLE.id) {
        node->disable();
    }
};

bool CarduinoNodeSerialEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
