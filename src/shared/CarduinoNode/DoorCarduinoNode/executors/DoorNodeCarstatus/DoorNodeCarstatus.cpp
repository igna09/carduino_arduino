#include "DoorNodeCarstatus.h"

DoorNodeCarstatus::DoorNodeCarstatus() : CarduinoNodeExecutorInterface(&Category::CAR_STATUS, Carstatus::REVERSE.id) {};

void DoorNodeCarstatus::execute(CarduinoNode *node, CanbusMessage *message) {
    DoorCarduinoNode *doorCarduinoNode = (DoorCarduinoNode*) node;
    CarstatusMessage *carstatusMessage = new CarstatusMessage(&CanbusMessageType::BOOL, message);
    bool isReverse = carstatusMessage->getBoolValue();
    doorCarduinoNode->reverse = isReverse;
    if(isReverse) {
        doorCarduinoNode->startMoveMirrorsDown();
    } else {
        doorCarduinoNode->startMoveMirrorsUp();
    }
};
