#include "HelloTrackerExecutor.h"

void HelloTrackerExecutor::execute(CarduinoNode *node, Message *message) {
    if (message->destination != node->_id || message->event->id != EV_HELLO) {
        return;
    }

    // L'executor è registrato solo su MainNode (vedi MainNode::MainNode),
    // quindi il cast è sicuro: EV_HELLO arriva sempre con destination ==
    // MAIN (vedi BootExecutor), che è l'unico nodo a montare questo executor.
    auto *main = static_cast<MainNode*>(node);

    uint8_t senderId = std::get<0>(static_cast<EventMulti<uint8_t>*>(message->event)->values);
    main->recordHello(senderId);
}