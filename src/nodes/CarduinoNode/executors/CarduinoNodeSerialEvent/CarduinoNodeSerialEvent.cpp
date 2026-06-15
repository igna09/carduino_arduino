#include "CarduinoNodeSerialEvent.h"

CarduinoNodeSerialEvent::CarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    /**
     * testing purpose, to be removed in production
     */
    if(message->messageId == Event::TEST.id) {
        node->test();
    }
    
    if(message->messageId == Event::ENABLE.id) {
        node->enable();
    } else if(message->messageId == Event::DISABLE.id) {
        node->disable();
    }

    EventMessage *eventMessage = new EventMessage(message);
    if(eventMessage->getIntValue() != node->id || eventMessage->getIntValue() == ALL_NODES) {
        node->sendCanbusMessage(eventMessage);
    }
    if(
        message->messageId == Event::RESTART.id
        && (eventMessage->getIntValue() == node->id || eventMessage->getIntValue() == ALL_NODES)
    ) {
        node->delayTask(1000, [&](){
            node->restart();
        });
    }
    delete eventMessage;
};

bool CarduinoNodeSerialEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
