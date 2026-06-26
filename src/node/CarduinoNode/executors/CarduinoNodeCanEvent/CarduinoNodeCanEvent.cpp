#include "CarduinoNodeCanEvent.h"

CarduinoNodeCanEvent::CarduinoNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeCanEvent::execute(CarduinoNode *node, Message *message) {

    if(message->destination == node->_id || message->destination == Node::BROADCAST.id) {
        if(message->event->id == EV_ENABLE) {
            node->enable();
        } else if(message->event->id == EV_DISABLE) {
            node->disable();
        } else if(message->event->id == EV_ENABLE_INTERRUPT) {
            node->enableInterrupt();
        } else if(message->event->id == EV_DISABLE_INTERRUPT) {
            node->disableInterrupt();
        } else if(message->event->id == EV_RESET_WEBAPP) {
            // node->resetWebapp();
        } else if(message->event->id == EV_RESTART) {
            node->delayTask(1000, [&](){
                node->restart();
            });
        } else if(message->event->id == EV_GET_HELLOS) {
            Message helloMessage(Priority::L.id, Node::MAIN.id, EventRegistry::createById(EV_HELLO));
            node->sendMessage(helloMessage);
        }
    }

    if(message->event->id == EV_HEARTBEAT) {
        node->heartbeatReceived();
    }
};

bool CarduinoNodeCanEvent::canExecute(CarduinoNode *node, Message *message) {
    return node->isEnabled || (
        message->event->id == EV_GET_HELLOS
        || message->event->id == EV_ENABLE
        || message->event->id == EV_DISABLE
        || message->event->id == EV_HEARTBEAT
    );
}
