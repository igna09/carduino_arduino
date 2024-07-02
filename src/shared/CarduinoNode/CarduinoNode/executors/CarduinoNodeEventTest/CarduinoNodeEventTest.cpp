#include "CarduinoNodeEventTest.h"

CarduinoNodeEventTest::CarduinoNodeEventTest() : CarduinoNodeExecutorInterface(&Category::EVENT, Event::TEST.id) {};

void CarduinoNodeEventTest::execute(CarduinoNode *node, CanbusMessage *message) {
    node->test();
};
