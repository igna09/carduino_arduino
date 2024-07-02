#include "CommunicationCarduinoNodeEvents.h"

CommunicationCarduinoNodeEvents::CommunicationCarduinoNodeEvents() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void CommunicationCarduinoNodeEvents::execute(CarduinoNode *node, CanbusMessage *message) {
    CommunicationCarduinoNode* communicationNode = (CommunicationCarduinoNode*) node;
    EventMessage *eventMessage = new EventMessage(message);

    if(eventMessage->getIntValue() == node->id || eventMessage->getIntValue() == ALL_NODES) {
        if(eventMessage->event->id == Event::ENABLE_NEW_BLE_PAIRING.id) {
            communicationNode->enableNewPairing();
        } else if(eventMessage->event->id == Event::DISABLE_NEW_BLE_PAIRING.id) {
            communicationNode->disableNewPairing();
        }
    }

    delete eventMessage;
};
