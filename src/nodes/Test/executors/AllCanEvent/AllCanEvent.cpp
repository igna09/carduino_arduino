#include "AllCanEvent.h"

AllCanEvent::AllCanEvent() : CarduinoNodeExecutorInterface() {};

void AllCanEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    // Category *category = (Category*)Category::getValueById(message->categoryId);
    // if(category->createSpecializedCopyFunction != nullptr) {
    //     CanbusMessage *specialized = category->createSpecializedCopyFunction(message);
    //     node->printlnWrapper(specialized->toSerialHumanString());
    //     delete specialized;
    // } else {
    //     node->printlnWrapper("Received " + message->toSerialHumanString());
    // }
    node->printlnWrapper("Received " + message->toSerialHumanString());
};

bool AllCanEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
