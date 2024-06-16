#include "MainNodeCanLog.h"

MainNodeCanLog::MainNodeCanLog() : CarduinoNodeExecutorInterface(&Category::LOG) {};

void MainNodeCanLog::execute(CarduinoNode *node, CanbusMessage *message) {
    if(node->isEnabled) {
        LogMessage *logMessage = new LogMessage(message);
        MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*)node;
        
        mainCarduinoNode->sendSerialMessage(logMessage);

        delete logMessage;
    }
};
