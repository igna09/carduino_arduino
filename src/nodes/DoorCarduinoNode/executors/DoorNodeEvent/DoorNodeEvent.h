#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"
#include "nodes/DoorCarduinoNode/DoorCarduinoNode.h"
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
