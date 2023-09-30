#include "TypedCanbusMessage.h"

TypedCanbusMessage::TypedCanbusMessage() : CanbusMessage() {};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, unsigned long id, uint8_t *payload, uint8_t payloadLength) : CanbusMessage(id, payload, payloadLength) {
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

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, int value) : CanbusMessage(id, convertIntegerToByteArray(value), 4) {
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::INT;

    this->value.intValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, float value) : CanbusMessage(id, convertFloatToByteArray(value), 5) {
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::FLOAT;

    this->value.floatValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, bool value) : CanbusMessage(id, convertBoolToByteArray(value), 1) {
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::BOOL;

    this->value.boolValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, CanbusMessage *message) : CanbusMessage(*message) {
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

int TypedCanbusMessage::getIntValue() {
    return this->value.intValue;
};

float TypedCanbusMessage::getFloatValue() {
    return this->value.floatValue;
};

bool TypedCanbusMessage::getBoolValue() {
    return this->value.boolValue;
};

String TypedCanbusMessage::getValueToString() {
    if(this->type->id == CanbusMessageType::BOOL.id) {
        return this->getBoolValue() ? "true" : "false";
    } else if(this->type->id == CanbusMessageType::INT.id) {
        return String(this->getIntValue());
    } else if(this->type->id == CanbusMessageType::FLOAT.id) {
        return String(this->getFloatValue());
    }
    return "";
};
