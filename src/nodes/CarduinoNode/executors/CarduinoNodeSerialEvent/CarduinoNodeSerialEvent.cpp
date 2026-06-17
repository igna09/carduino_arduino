#include "CarduinoNodeSerialEvent.h"

CarduinoNodeSerialEvent::CarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    /**
     * testing purpose, to be removed in production
     */
    if(message->eventId == EventEnum::TEST.id) {
        node->test();
    }
    
    if(message->eventId == EventEnum::ENABLE.id) {
        node->enable();
    } else if(message->eventId == EventEnum::DISABLE.id) {
        node->disable();
    }

    if(message->targetNode != node->id || message->targetNode == NODE_BROADCAST) {
        node->sendCanbusMessage(message);
    }

    if(
        message->eventId == EventEnum::RESTART.id
        && (message->targetNode == node->id || message->targetNode == NODE_BROADCAST)
    ) {
        node->delayTask(1000, [&](){
            node->restart();
        });
    }
};

bool CarduinoNodeSerialEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
