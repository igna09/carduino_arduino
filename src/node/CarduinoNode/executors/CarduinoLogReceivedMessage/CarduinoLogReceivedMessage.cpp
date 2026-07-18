#include "CarduinoLogReceivedMessage.h"

CarduinoLogReceivedMessage::CarduinoLogReceivedMessage(const char* loggerType) : CarduinoNodeExecutorInterface() {
    this->loggerType = loggerType;
};

void CarduinoLogReceivedMessage::execute(CarduinoNode *node, Message *message) {
    NLOGI("Received %s message %s", loggerType, message->toString().c_str());
};

bool CarduinoLogReceivedMessage::canExecute(CarduinoNode *node, Message *message) {
    auto settingPtr = node->getSetting<bool>(&Setting::LOG_SND_RCV_MSG);
    return settingPtr != nullptr && settingPtr->value;
}