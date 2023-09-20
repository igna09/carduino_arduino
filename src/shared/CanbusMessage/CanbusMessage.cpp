#include "CanbusMessage.h"

CanbusMessage::CanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength) {
    this->id = id;
    this->payload = payload;
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

CanbusMessage::CanbusMessage() {};
