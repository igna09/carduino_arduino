#include "LogMessage.h"

LogMessage::LogMessage(uint8_t nodeId, uint8_t logId, int value) : CanbusMessage(), ValueTypedBase(value) {
    this->id = CarduinoNode::generateId(Category::LOG, nodeId);
    this->logId = logId;
    this->nodeId = nodeId;
    
    uint8_t payloadLength = 5;
    uint8_t *intToByteArray = convertValueToByteArray(value);

    this->payload = new uint8_t[payloadLength];
    this->payload[0] = logId;
    for(uint8_t i = 1; i < payloadLength; i++) {
        this->payload[i] = intToByteArray[i - 1];
    }
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

LogMessage::LogMessage(uint8_t nodeId, uint8_t logId, float value) : CanbusMessage(), ValueTypedBase(value) {
    this->id = CarduinoNode::generateId(Category::LOG, nodeId);
    this->logId = logId;
    this->nodeId = nodeId;
    
    uint8_t payloadLength = 6;
    uint8_t *intToByteArray = convertValueToByteArray(value);

    this->payload = new uint8_t[payloadLength];
    this->payload[0] = logId;
    for(uint8_t i = 1; i < payloadLength; i++) {
        this->payload[i] = intToByteArray[i - 1];
    }
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

LogMessage::LogMessage(uint8_t nodeId, uint8_t logId, bool value) : CanbusMessage(), ValueTypedBase(value) {
    this->id = CarduinoNode::generateId(Category::LOG, nodeId);
    this->logId = logId;
    this->nodeId = nodeId;
    
    uint8_t payloadLength = 2;
    uint8_t *intToByteArray = convertValueToByteArray(value);

    this->payload = new uint8_t[payloadLength];
    this->payload[0] = logId;
    for(uint8_t i = 1; i < payloadLength; i++) {
        this->payload[i] = intToByteArray[i - 1];
    }
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

LogMessage::LogMessage(CanbusMessage *canbusMessage) : CanbusMessage(*canbusMessage) {
    this->id = CarduinoNode::generateId(Category::LOG, nodeId);
    this->logId = this->payload[0];
    this->nodeId = this->messageId;
    
    if(this->payloadLength == BOOL_BYTES_SIZE + 1) {
        uint8_t *valueBytes = new uint8_t[BOOL_BYTES_SIZE];
        for(uint8_t i = 0; i < BOOL_BYTES_SIZE; i++) {
            valueBytes[i] = this->payload[i + 1];
        }
        this->type = &CanbusMessageType::BOOL;
        this->value.boolValue = convertByteArrayToBool(valueBytes, BOOL_BYTES_SIZE);
        delete valueBytes;
    } else if(this->payloadLength == INT_BYTES_SIZE + 1) {
        uint8_t *valueBytes = new uint8_t[INT_BYTES_SIZE];
        for(uint8_t i = 0; i < INT_BYTES_SIZE; i++) {
            valueBytes[i] = this->payload[i + 1];
        }
        this->type = &CanbusMessageType::INT;
        this->value.intValue = convertByteArrayToInt(valueBytes, INT_BYTES_SIZE);
        delete valueBytes;
    } else if(this->payloadLength == FLOAT_BYTES_SIZE + 1) {
        uint8_t *valueBytes = new uint8_t[FLOAT_BYTES_SIZE];
        for(uint8_t i = 0; i < FLOAT_BYTES_SIZE; i++) {
            valueBytes[i] = this->payload[i + 1];
        }
        this->type = &CanbusMessageType::FLOAT;
        this->value.floatValue = convertByteArrayToFloat(valueBytes, FLOAT_BYTES_SIZE);
        delete valueBytes;
    }
};

String LogMessage::toSerialHumanString() {
    String s = "";
    s += Category::getValueById(this->categoryId)->name;
    s += ";";
    s += this->nodeId;
    s += ";";
    s += this->logId;
    s += "-";
    s += this->getValueToString();
    s += ";";
    return s;
};

String LogMessage::toSerialString() {
    String s = "";
    s += this->categoryId;
    s += ";";
    s += this->nodeId;
    s += ";";
    s += this->logId;
    s += "@";
    s += this->getValueToString();
    s += ";";
    return s;
};

LogMessage* LogMessage::createSpecializedCopy(CanbusMessage* canbusMessage) {
    return new LogMessage(canbusMessage);
};
