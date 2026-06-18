#include "CarduinoNodeCanEvent.h"

CarduinoNodeCanEvent::CarduinoNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {

    if(message->destination == node->id || message->destination == NODE_BROADCAST) {
        if(message->event->id == EV_ENABLE) {
            node->enable();
        } else if(message->event->id == EV_DISABLE) {
            node->disable();
        } else if(message->event->id == EV_ENABLE_INTERRUPT) {
            node->enableInterrupt();
        } else if(message->event->id == EV_DISABLE_INTERRUPT) {
            node->disableInterrupt();
        } else if(message->event->id == EV_RESET_WEBAPP) {
            node->resetWebapp();
        } else if(message->event->id == EV_RESTART) {
            node->delayTask(1000, [&](){
                node->restart();
            });
        } else if(message->event->id == EV_GET_HELLOS) {
            CanbusMessage *helloMessage = new CanbusMessage(LOW_PRIORITY, MAIN_NODE_ADDRESS, EventRegistry::createById(EV_HELLO));
            node->sendCanbusMessage(helloMessage);
            delete helloMessage;
        }
    }

    if(message->event->id == EV_HEARTBEAT) {
        node->lastTimeReceivedHeartbeat = millis();
    }
};

bool CarduinoNodeCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled || (
        message->event->id == EV_GET_HELLOS
        || message->event->id == EV_ENABLE
        || message->event->id == EV_DISABLE
        || message->event->id == EV_HEARTBEAT
    );
}
