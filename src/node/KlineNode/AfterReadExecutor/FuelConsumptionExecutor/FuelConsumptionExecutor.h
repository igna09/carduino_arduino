#pragma once

#include "AfterReadExecutorInterface.h"
#include "ValueToRead.h"
#include "CarduinoNode.h"
#include "Message.h"
#include "Priority.h"
#include "Node.h"
#include "NodeLog.h"

class FuelConsumptionExecutor : public AfterReadExecutorInterface {
    public:
        void execute(CarduinoNode *carduinoNode);
};