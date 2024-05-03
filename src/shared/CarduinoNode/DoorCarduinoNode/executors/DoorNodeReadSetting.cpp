#include "DoorNodeReadSetting.h"

DoorNodeReadSetting::DoorNodeReadSetting() : CarduinoNodeExecutorInterface(&Category::READ_SETTING) {};

void DoorNodeReadSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    ReadSettingMessage *settingMessage = new ReadSettingMessage(&Setting::ON_REVERSE_LOWER_MIRRORS, doorCarduinoNode->lowerMirrorsOnReverse);
    node->sendCanbusMessage(settingMessage);
};
