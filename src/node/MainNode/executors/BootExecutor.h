#pragma once

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "EventMulti.h"

class BootExecutor: public CarduinoNodeExecutorInterface {
public:
    BootExecutor() : CarduinoNodeExecutorInterface({EV_HELLO, EV_TIME_SYNC_REQUEST}) {};

    void execute(CarduinoNode *node, Message *message) {
        if(message->destination == node->_id ) {
            if(message->event->id == EV_HELLO) {
                // Arriva solo su MAIN (destination è sempre MAIN per costruzione).
                // Il mittente è nel payload: rispondiamo con ENABLE diretto a lui,
                // che a sua volta fa scattare startTimeSync() sul nodo richiedente.
                uint8_t senderId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
                Message enableMessage(Priority::L.id, senderId, EventRegistry::createById(EV_ENABLE));
                node->sendMessage(enableMessage);
            } else if(message->event->id == EV_TIME_SYNC_REQUEST) {
                // Arriva solo su MAIN. Il mittente è nel payload, serve per
                // sapere a chi indirizzare la TIME_SYNC_RESPONSE.
                uint8_t requesterId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
                node->handleTimeSyncRequest(requesterId);
            }
        }
    };
};