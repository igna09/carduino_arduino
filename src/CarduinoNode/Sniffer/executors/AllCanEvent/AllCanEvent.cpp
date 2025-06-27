#include "AllCanEvent.h"

AllCanEvent::AllCanEvent() : CarduinoNodeExecutorInterface() {};

void AllCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper(message->toSerialHumanString());
    Category *category = (Category*)Category::getValueById(message->categoryId);
    if(category->createSpecializedCopyFunction != nullptr) {
        CanbusMessage *specialized = category->createSpecializedCopyFunction(message);
        node->printlnWrapper("AllCanEvent::execute received " + specialized->toSerialHumanString());
        delete specialized;
    } else {
        node->printlnWrapper("AllCanEvent::execute received " + message->toSerialHumanString());
    }
};

bool AllCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
