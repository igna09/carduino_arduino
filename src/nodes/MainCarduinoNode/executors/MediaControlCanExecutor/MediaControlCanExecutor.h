#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "nodes/MainCarduinoNode/MainCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"

class MediaControlCanExecutor : public CarduinoNodeExecutorInterface {
    public:
        MediaControlCanExecutor();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
