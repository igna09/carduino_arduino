#include "MainNodeCanLog.h"

MainNodeCanLog::MainNodeCanLog() : CarduinoNodeExecutorInterface(&Category::LOG) {};

void MainNodeCanLog::execute(CarduinoNode *node, CanbusMessage *message) {
    LogMessage *logMessage = new LogMessage(message);
    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*)node;
    
    mainCarduinoNode->sendSerialMessage(logMessage);

    delete logMessage;
};
