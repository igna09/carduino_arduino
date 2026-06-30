#include "BootExecutor.h"

BootExecutor::BootExecutor() : CarduinoNodeExecutorInterface({EV_HELLO, EV_TIME_SYNC_REQUEST}) {};

void BootExecutor::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);

    if(message->destination == main->_id ) {
        if(message->event->id == EV_HELLO) {
            // Arriva solo su MAIN (destination è sempre MAIN per costruzione).
            // Il mittente è nel payload: rispondiamo con ENABLE diretto a lui,
            // che a sua volta fa scattare startTimeSync() sul nodo richiedente.
            uint8_t senderId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
            Message enableMessage(Priority::L.id, senderId, EventRegistry::createById(EV_ENABLE));
            main->sendMessage(enableMessage);
        } else if(message->event->id == EV_TIME_SYNC_REQUEST) {
            // Arriva solo su MAIN. Il mittente è nel payload, serve per
            // sapere a chi indirizzare la TIME_SYNC_RESPONSE.
            uint8_t requesterId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
            main->handleTimeSyncRequest(requesterId);
        }
    }
};