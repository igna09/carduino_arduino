#include "FuelConsumptionExecutor.h"
#include "KlineNode.h"

void FuelConsumptionExecutor::execute(CarduinoNode *carduinoNode) {
    auto* klineNode = static_cast<KlineNode*>(carduinoNode);

    int speed = klineNode->getLastValue<int>(ValueToRead::SPEED.id); 
    float consumption = klineNode->getLastValue<float>(ValueToRead::FUEL_CONSUMPTION.id);

    float value = (speed != 0 && consumption != 0) ? speed / consumption : 0;

    auto* ev = static_cast<EventMulti<float>*>(EventRegistry::createById(EV_FUEL_CONSUMPTION));
    std::get<0>(ev->values) = value;
    Message c(Priority::L.id, Node::BROADCAST.id, ev);
    carduinoNode->sendMessage(c);
};