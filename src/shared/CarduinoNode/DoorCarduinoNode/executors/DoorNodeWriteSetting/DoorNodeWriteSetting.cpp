#include "DoorNodeWriteSetting.h"

DoorNodeWriteSetting::DoorNodeWriteSetting() : CarduinoNodeExecutorInterface(&Category::WRITE_SETTING) {};

void DoorNodeWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    WriteSettingMessage *settingMessage = new WriteSettingMessage(*message);

    if(settingMessage->setting->id == Setting::ON_REVERSE_LOWER_MIRRORS.id) {
        doorCarduinoNode->lowerMirrorsOnReverse = settingMessage->getBoolValue();
    } else if(settingMessage->setting->id == Setting::AUTO_CLOSE_REARVIEW_MIRRORS.id) {
        doorCarduinoNode->autoCloseMirrors = settingMessage->getBoolValue();
    }

    delete settingMessage;
};
