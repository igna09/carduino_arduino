#include "AllCanEvent.h"

AllCanEvent::AllCanEvent() : CarduinoNodeExecutorInterface() {};

void AllCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper(message->toSerialHumanString());
};

bool AllCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
