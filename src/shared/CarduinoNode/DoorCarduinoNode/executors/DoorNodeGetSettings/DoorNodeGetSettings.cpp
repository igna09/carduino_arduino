#include "DoorNodeGetSettings.h"

DoorNodeGetSettings::DoorNodeGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void DoorNodeGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    ReadSettingMessage *settingMessage = new ReadSettingMessage(&Setting::ON_REVERSE_LOWER_MIRRORS, doorCarduinoNode->lowerMirrorsOnReverse);
    
    node->sendCanbusMessage(settingMessage);

    delete settingMessage;
};
