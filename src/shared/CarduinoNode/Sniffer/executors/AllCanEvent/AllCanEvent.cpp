#include "AllCanEvent.h"

AllCanEvent::AllCanEvent() : CarduinoNodeExecutorInterface() {};

void MediaControlCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper(message->toSerialHumanString());
};

bool MediaControlCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
