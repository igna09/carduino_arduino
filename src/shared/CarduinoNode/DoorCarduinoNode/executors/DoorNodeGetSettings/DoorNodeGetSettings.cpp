#include "DoorNodeGetSettings.h"

DoorNodeGetSettings::DoorNodeGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void DoorNodeGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;

    ReadSettingMessage *onReverseLowerMirrorsSettingMessage = new ReadSettingMessage(&Setting::ON_REVERSE_LOWER_MIRRORS, doorCarduinoNode->lowerMirrorsOnReverse);
    node->sendCanbusMessage(onReverseLowerMirrorsSettingMessage);
    delete onReverseLowerMirrorsSettingMessage;

    ReadSettingMessage *autoCloseRearviewMirrorsSettingMessage = new ReadSettingMessage(&Setting::AUTO_CLOSE_REARVIEW_MIRRORS, doorCarduinoNode->autoCloseMirrors);
    node->sendCanbusMessage(autoCloseRearviewMirrorsSettingMessage);
    delete autoCloseRearviewMirrorsSettingMessage;
};
