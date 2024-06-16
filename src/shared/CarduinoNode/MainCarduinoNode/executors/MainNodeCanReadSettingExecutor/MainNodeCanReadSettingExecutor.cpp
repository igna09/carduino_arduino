#include "MainNodeCanReadSettingExecutor.h"

MainNodeCanReadSettingExecutor::MainNodeCanReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTING) {};

void MainNodeCanReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    if(node->isEnabled) {
        MainCarduinoNode *mainCarduinoNode = (MainCarduinoNode*) node;
        
        SettingMessage *settingMessage = new SettingMessage(message);
        mainCarduinoNode->sendSerialMessage(settingMessage);
        delete settingMessage;
    }
};
