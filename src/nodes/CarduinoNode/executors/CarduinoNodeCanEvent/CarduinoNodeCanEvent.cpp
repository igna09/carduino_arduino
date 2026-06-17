#include "CarduinoNodeCanEvent.h"

CarduinoNodeCanEvent::CarduinoNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {

    if(message->targetNode == node->id || message->targetNode == NODE_BROADCAST) {
        if(message->eventId == EventEnum::ENABLE.id) {
            node->enable();
        } else if(message->eventId == EventEnum::DISABLE.id) {
            node->disable();
        } else if(message->eventId == EventEnum::ENABLE_INTERRUPT.id) {
            node->enableInterrupt();
        } else if(message->eventId == EventEnum::DISABLE_INTERRUPT.id) {
            node->disableInterrupt();
        } else if(message->eventId == EventEnum::RESET_WEBAPP.id) {
            node->resetWebapp();
        } else if(message->eventId == EventEnum::RESTART.id) {
            node->delayTask(1000, [&](){
                node->restart();
            });
        } else if(message->eventId == EventEnum::GET_HELLOS.id) {
            CanbusMessage *helloMessage = new CanbusMessage();
            helloMessage->eventId = EventEnum::HELLO.id;
            helloMessage->targetNode = node->id;
            node->sendCanbusMessage(helloMessage);
            delete helloMessage;
        }
    }

    if(message->eventId == EventEnum::HEARTBEAT.id) {
        node->lastTimeReceivedHeartbeat = millis();
    }
};

bool CarduinoNodeCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled || (
        message->eventId == EventEnum::GET_HELLOS.id
        || message->eventId == EventEnum::ENABLE.id
        || message->eventId == EventEnum::DISABLE.id
        || message->eventId == EventEnum::HEARTBEAT.id
    );
}
