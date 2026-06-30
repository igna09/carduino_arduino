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
            if(node->isEnabled) return;
            // HELLO ora porta l'id del mittente nel payload (1 byte), così
            // MAIN sa a chi rispondere con ENABLE.
            node->sendHello();
        } else if(message->event->id == EV_TIME_SYNC_RESPONSE) {
            // Arriva sul nodo che aveva richiesto il sync (destination==lui).
            auto *ev = static_cast<EventMulti<uint32_t, uint32_t>*>(message->event);
            uint32_t t2 = std::get<0>(ev->values);
            uint32_t t3 = std::get<1>(ev->values);
            node->handleTimeSyncResponse(t2, t3);
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
        || message->event->id == EV_HELLO
        || message->event->id == EV_TIME_SYNC_REQUEST
        || message->event->id == EV_TIME_SYNC_RESPONSE
    );
}