#include "CanbusMessage.h"

CanbusMessage::CanbusMessage(uint16_t id, uint8_t *payload, uint8_t payloadLength) {
    this->id = id;
    this->payload = new uint8_t[payloadLength];
    for(uint8_t i = 0; i < payloadLength; i++) {
        this->payload[i] = payload[i];
    }
    this->payloadLength = payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

CanbusMessage::CanbusMessage(CanbusMessage &message) {
    this->id = message.id;
    this->payload = new uint8_t[message.payloadLength];
    for(uint8_t i = 0; i < message.payloadLength; i++) {
        this->payload[i] = message.payload[i];
    }
    // this->payload = message.payload;
    this->payloadLength = message.payloadLength;
    this->categoryId = this->id>>8;
    this->messageId = (this->id<<3)>>3;
};

CanbusMessage::CanbusMessage() {};

CanbusMessage::~CanbusMessage() {
    delete[] payload;
}
