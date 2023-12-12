#pragma once

#include <Arduino.h>
#include "AfterReadExecutorInterface.h"
#include "../ValueToReadEnum.h"
#include "../../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "../../CarduinoNode/CarduinoNode.h"

class FuelConsumptionExecutor : public AfterReadExecutorInterface {
    public:
        void execute(CarduinoNode *carduinoNode) {
            float v;
            if(ValueToReadEnum::SPEED.lastReadValue.intValue != 0 && ValueToReadEnum::FUEL_CONSUMPTION.lastReadValue.floatValue != 0) {
                v = ValueToReadEnum::SPEED.lastReadValue.intValue / ValueToReadEnum::FUEL_CONSUMPTION.lastReadValue.floatValue;
            } else {
                v = 0;
            }
            CarstatusMessage *c = new CarstatusMessage(&Carstatus::FUEL_CONSUMPTION, v);
            carduinoNode->sendCanbusMessage(c);
            delete c;
        };
};