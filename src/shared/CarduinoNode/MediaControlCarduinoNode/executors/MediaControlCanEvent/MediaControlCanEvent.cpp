#include "MediaControlCanEvent.h"

MediaControlCanEvent::MediaControlCanEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MediaControlCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    EventMessage *eventMessage = new EventMessage(message);

    MediaControlCarduinoNode* mediaControlCarduinoNode = (MediaControlCarduinoNode*) node;

    if(eventMessage->event->id == Event::WARNING_SEVERITY_LOW.id) {
        mediaControlCarduinoNode->playTone(&Event::WARNING_SEVERITY_LOW);
    } else if(eventMessage->event->id == Event::WARNING_SEVERITY_MEDIUM.id) {
        mediaControlCarduinoNode->playTone(&Event::WARNING_SEVERITY_MEDIUM);
    } else if(eventMessage->event->id == Event::WARNING_SEVERITY_HIGH.id) {
        mediaControlCarduinoNode->playTone(&Event::WARNING_SEVERITY_HIGH);
    } else if(eventMessage->event->id == Event::SWC_PAIR.id) {
        mediaControlCarduinoNode->readyToStartSwcPairing();
    }

    delete eventMessage;
};

bool MediaControlCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled;
}
