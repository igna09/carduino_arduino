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

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, int value) : CanbusMessage(id, nullptr, 4) {
    uint8_t *bytes = new uint8_t[4];
    convertIntegerToByteArray(bytes, value);
    this->payload = bytes;

    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::INT;

    this->value.intValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, float value) : CanbusMessage(id, nullptr, 5) {
    uint8_t *bytes = new uint8_t[5];
    convertFloatToByteArray(bytes, value);
    this->payload = bytes;

    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::FLOAT;

    this->value.floatValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(unsigned long id, bool value) : CanbusMessage(id, nullptr, 1) {
    uint8_t *bytes = new uint8_t[1];
    convertBoolToByteArray(bytes, value);
    this->payload = bytes;
    
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = &CanbusMessageType::BOOL;

    this->value.boolValue = value;
};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, CanbusMessage *message) : CanbusMessage(message) {
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

TypedCanbusMessage::~TypedCanbusMessage() {
    delete[] payload;
}

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
