#include "MainNodeCanReadSettingExecutor.h"

MainNodeCanReadSettingExecutor::MainNodeCanReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTING) {};

void MainNodeCanReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*) node;
    
    ReadSettingMessage *readSettingMessage = new ReadSettingMessage(message);
    mainCarduinoNode->sendSerialMessage(readSettingMessage);
    delete readSettingMessage;
};
