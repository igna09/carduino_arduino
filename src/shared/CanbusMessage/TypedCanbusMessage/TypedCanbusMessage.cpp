#include "TypedCanbusMessage.h"

TypedCanbusMessage::TypedCanbusMessage() : CanbusMessage() {};

TypedCanbusMessage::TypedCanbusMessage(CanbusMessageType *type, unsigned long id, uint8_t *payload, uint8_t payloadLength) : CanbusMessage(id, payload, payloadLength) {
    this->carstatus = Carstatus::getValueById(this->messageId);
    this->category = Category::getValueById(this->categoryId);
    this->type = type;

    if(*type == CanbusMessageType::BOOL) {
        this->value.boolValue = convertByteArrayToBool(payload, payloadLength);
    } else if(*type == CanbusMessageType::INT) {
        this->value.intValue = convertByteArrayToInt(payload, payloadLength);
    } else if(*type == CanbusMessageType::FLOAT) {
        this->value.floatValue = convertByteArrayToFloat(payload, payloadLength);
    }
};


int TypedCanbusMessage::getIntValue() {
    return this->value.intValue;
};

float TypedCanbusMessage::getFloatValue() {
    return this->value.floatValue;
};

bool TypedCanbusMessage::getBoolValue() {
    return this->value.boolValue;
};
