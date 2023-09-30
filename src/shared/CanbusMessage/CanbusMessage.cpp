#include "CanbusMessage.h"

CanbusMessage::CanbusMessage(uint16_t id, uint8_t *payload, uint8_t payloadLength) {
    Serial.println("CanbusMessage::CanbusMessage");
    this->id = id;
    this->payload = payload;
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

CanbusMessage::CanbusMessage(CanbusMessage *message) {
    this->id = message->id;
    this->payload = message->payload;
    this->payloadLength = message->payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

CanbusMessage::CanbusMessage() {};
