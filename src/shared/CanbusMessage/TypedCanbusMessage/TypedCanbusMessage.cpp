#include "TypedCanbusMessage.h"

TypedCanbusMessage::TypedCanbusMessage() : CanbusMessage() {};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, unsigned long id, uint8_t *payload, uint8_t payloadLength) : CanbusMessage(id, payload, payloadLength), ValueTypedBase() {
    this->category = (Category*) Category::getValueById(this->categoryId);
    this->type = type;

    if(this->type->id == CanbusMessageType::BOOL.id) {
        this->value.boolValue = convertByteArrayToBool(this->payload, this->payloadLength);
    } else if(this->type->id == CanbusMessageType::INT.id) {
        this->value.intValue = convertByteArrayToInt(this->payload, this->payloadLength);
    } else if(this->type->id == CanbusMessageType::FLOAT.id) {
        this->value.floatValue = convertByteArrayToFloat(this->payload, this->payloadLength);
    }
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, int value) : CanbusMessage(id, convertValueToByteArray(value), 4), ValueTypedBase(value) {
    this->category = (Category*)Category::getValueById(this->categoryId);
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, float value) : CanbusMessage(id, convertValueToByteArray(value), 5), ValueTypedBase(value) {
    this->category = (Category*)Category::getValueById(this->categoryId);
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, bool value) : CanbusMessage(id, convertValueToByteArray(value), 1), ValueTypedBase(value) {
    this->category = (Category*)Category::getValueById(this->categoryId);
};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, CanbusMessage *message) : CanbusMessage(*message), ValueTypedBase() {
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = type;

    if(this->type->id == CanbusMessageType::BOOL.id) {
        this->value.boolValue = convertByteArrayToBool(payload, payloadLength);
    } else if(this->type->id == CanbusMessageType::INT.id) {
        this->value.intValue = convertByteArrayToInt(payload, payloadLength);
    } else if(this->type->id == CanbusMessageType::FLOAT.id) {
        this->value.floatValue = convertByteArrayToFloat(payload, payloadLength);
    }
};
