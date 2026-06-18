#pragma once

#include <Arduino.h>
#include "AfterReadExecutorInterface.h"
#include "../ValueToReadEnum.h"
// #include "../../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"
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

            auto* ev = static_cast<EventMulti<bool>*>(EventRegistry::createById(EV_FUEL_CONSUMPTION));
            std::get<0>(ev->values) = v;
            CanbusMessage *c = new CanbusMessage(LOW_PRIORITY, NODE_BROADCAST, ev);
            carduinoNode->sendCanbusMessage(c);
            delete c;
        };
};