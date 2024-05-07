#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"

class MediaControlExecutor : public CarduinoNodeExecutorInterface {
    public:
        MediaControlExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
};
