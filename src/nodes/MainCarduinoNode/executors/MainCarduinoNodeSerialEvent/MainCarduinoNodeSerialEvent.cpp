#include "MainCarduinoNodeSerialEvent.h"

MainCarduinoNodeSerialEvent::MainCarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainCarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    // MainCarduinoNode* mainCarduinoNode = (MainCarduinoNode*) node;
    // if(message->messageId == Event::SWC_PAIR.id) {
    //     mainCarduinoNode->sendCanbusMessage(message);
    // }
    node->sendCanbusMessage(message);
};

bool MainCarduinoNodeSerialEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled;
}
