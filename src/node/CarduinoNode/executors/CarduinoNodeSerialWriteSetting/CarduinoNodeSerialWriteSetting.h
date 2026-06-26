#pragma once

#include <cstdint>

#include "Message.h"
#include "CarduinoNode.h"
#include "CarduinoNodeExecutorInterface.h"
#include "Setting.h"
#include "EventMulti.h"
#include "MessageType.h"

// A differenza di CarduinoNodeExecutorInterface.h ed Executor.h, qui
// l'include completo di CarduinoNode.h è necessario: execute() chiama
// node->sendMessage(...) e node->putSettingValue(...), quindi serve la
// definizione completa della classe, non basta il puntatore. Questo .h
// può essere incluso solo dai .cpp che istanziano l'executor concreto
// (tipicamente il setup applicativo), non da Executor.h/CarduinoNode.h,
// altrimenti il ciclo si riapre da qui.
class CarduinoNodeSerialWriteSetting : public CarduinoNodeExecutorInterface {
    public:
        CarduinoNodeSerialWriteSetting();
        void execute(CarduinoNode *node, Message *message) override;
};