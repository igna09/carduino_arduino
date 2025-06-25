#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Category.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/CarduinoNode/MediaControlCarduinoNode/MediaControlCarduinoNode.h"
#include "shared/enums/Event.h"

class MediaControlCanEvent : public CarduinoNodeExecutorInterface {
    public:
        MediaControlCanEvent();
        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
