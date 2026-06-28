#include "CarduinoNodeCanEvent.h"

CarduinoNodeCanEvent::CarduinoNodeCanEvent() : CarduinoNodeExecutorInterface() {};

void CarduinoNodeCanEvent::execute(CarduinoNode *node, Message *message) {

    if(message->destination == node->_id || message->destination == Node::BROADCAST.id) {
        if(message->event->id == EV_ENABLE) {
            node->enable();
            // Il time sync parte solo sui nodi non-MAIN, in risposta al
            // proprio ENABLE (che MAIN manda dopo aver ricevuto il loro
            // HELLO). MAIN non ha bisogno di sincronizzarsi con se stesso.
            if (node->_id != Node::MAIN.id) {
                node->startTimeSync();
            }
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
            if(node->_id == Node::MAIN.id) return;
            // HELLO ora porta l'id del mittente nel payload (1 byte), così
            // MAIN sa a chi rispondere con ENABLE.
            Message helloMessage(Priority::L.id, Node::MAIN.id,
                                  new EventMulti<uint8_t>(EV_HELLO, "HELLO"));
            std::get<0>(static_cast<EventMulti<uint8_t>*>(helloMessage.event)->values) = node->_id;
            node->sendMessage(helloMessage);
        } else if(message->event->id == EV_TIME_SYNC_REQUEST) {
            // Arriva solo su MAIN. Il mittente è nel payload, serve per
            // sapere a chi indirizzare la TIME_SYNC_RESPONSE.
            uint8_t requesterId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
            node->handleTimeSyncRequest(requesterId);
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