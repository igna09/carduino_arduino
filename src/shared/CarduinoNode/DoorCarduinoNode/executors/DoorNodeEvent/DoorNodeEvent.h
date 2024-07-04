#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/DoorCarduinoNode/DoorCarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Event.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/enums/Category.h"

class DoorNodeEvent : public CarduinoNodeExecutorInterface {
    public:
        DoorNodeEvent();

        void execute(CarduinoNode *node, CanbusMessage *message);
        bool canExecute(CarduinoNode *node, CanbusMessage *message);
};
