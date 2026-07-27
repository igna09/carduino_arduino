#include "SpeedLimitSet.h"

SpeedLimitSet::SpeedLimitSet(): CarduinoNodeExecutorInterface({EV_SPEED_LIMIT_SET}) {};

void SpeedLimitSet::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);
    
    uint8_t limit = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);

    main->_speedWarn.setLimit(limit);
};
