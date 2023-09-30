#include "CarstatusExecutor.h"
#include "../../../enums/Setting.h"

CarstatusExecutor::CarstatusExecutor() : CarduinoNodeExecutorInterface(&Category::CAR_STATUS) {}

//TODO: check memory leak
void CarstatusExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    const Carstatus *c = (Carstatus*)Carstatus::getValueById(message->messageId);
    const CanbusMessageType t = c->type;

    CarstatusMessage *carstatusMessage = new CarstatusMessage(&t, message);

    ((MainCarduinoNode*)node)->sendSerialMessage(carstatusMessage);

    delete carstatusMessage;
}
