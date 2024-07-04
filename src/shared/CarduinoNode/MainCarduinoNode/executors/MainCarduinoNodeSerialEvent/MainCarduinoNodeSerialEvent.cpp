#include "MainCarduinoNodeSerialEvent.h"

MainCarduinoNodeSerialEvent::MainCarduinoNodeSerialEvent() : CarduinoNodeExecutorInterface(&Category::EVENT) {};

void MainCarduinoNodeSerialEvent::execute(CarduinoNode *node, CanbusMessage *message) {
    MainCarduinoNode* mainCarduinoNode = (MainCarduinoNode*) node;
    if(message->messageId == Event::SWC_PAIR.id) {
        mainCarduinoNode->startSwcPairing();
    }
};

bool MainCarduinoNodeSerialEvent::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled;
}
