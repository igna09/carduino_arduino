#pragma once

#include "AfterReadExecutorInterface.h"
#include "ValueToRead.h"
#include "CarduinoNode.h"
#include "Message.h"

class FuelConsumptionExecutor : public AfterReadExecutorInterface {
    public:
        void execute(CarduinoNode *carduinoNode) {
            float v;
            if(ValueToRead::SPEED.lastReadValue.intValue != 0 && ValueToRead::FUEL_CONSUMPTION.lastReadValue.floatValue != 0) {
                v = ValueToRead::SPEED.lastReadValue.intValue / ValueToRead::FUEL_CONSUMPTION.lastReadValue.floatValue;
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