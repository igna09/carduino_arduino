#include "SwcPairingEvent.h"

SwcPairingEvent::SwcPairingEvent(): CarduinoNodeExecutorInterface({EV_SWC_PAIR}) {};

void SwcPairingEvent::execute(CarduinoNode *node, Message *message) {
    auto *main = static_cast<MainNode*>(node);
    main->startSwcPairing();
};
