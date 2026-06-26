#include "CarduinoNodeSerialEvent.h"

CarduinoNodeSerialEvent::CarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeSerialEvent::execute(CarduinoNode *node, Message *message) {
    /**
     * testing purpose, to be removed in production
     */
    if(message->event->id == EV_TEST) {
        node->test();
    }
    
    if(message->event->id == EV_ENABLE) {
        node->enable();
    } else if(message->event->id == EV_DISABLE) {
        node->disable();
    }

    if(message->destination != node->id || message->destination == NODE_BROADCAST) {
        node->sendMessage(message);
    }

    if(
        message->event->id == EV_RESTART
        && (message->destination == node->id || message->destination == NODE_BROADCAST)
    ) {
        node->delayTask(1000, [&](){
            node->restart();
        });
    }
};
