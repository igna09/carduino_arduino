#include "AllMessageExecutor.h"

AllMessageExecutor::AllMessageExecutor() : CarduinoNodeExecutorInterface(nullptr) {};

void AllMessageExecutor::execute(CarduinoNode *node, CanbusMessage message) {
    //TODO: executor to execute on every message
};
