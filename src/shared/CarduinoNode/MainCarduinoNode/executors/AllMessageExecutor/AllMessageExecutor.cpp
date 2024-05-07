#include "AllMessageExecutor.h"

AllMessageExecutor::AllMessageExecutor() : CarduinoNodeExecutorInterface(nullptr) {};

void AllMessageExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    //TODO: executor to execute on every message
    Serial.print("Received message; message id ");
    Serial.print(message->id, BIN);
    Serial.print(", payload ");
    for(uint8_t i = 0; i < message->payloadLength; i++) {
        Serial.print(message->payload[i]);
    }
    Serial.println();
};
