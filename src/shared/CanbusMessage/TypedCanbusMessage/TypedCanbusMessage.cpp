#include "TypedCanbusMessage.h"

TypedCanbusMessage::TypedCanbusMessage() : CanbusMessage() {};

TypedCanbusMessage::TypedCanbusMessage(const CanbusMessageType *type, unsigned long id, uint8_t *payload, uint8_t payloadLength) : CanbusMessage(id, payload, payloadLength) {
    this->category = (Category*)Category::getValueById(this->categoryId);
    this->type = type;

    Serial.print("TypedCanbusMessage::TypedCanbusMessage this->type->name ");
    Serial.print(this->type->name);
    Serial.print(" CanbusMessageType::BOOL->name ");
    Serial.println(CanbusMessageType::BOOL->name);

    if(this->type->id == CanbusMessageType::BOOL->id) {
        this->value.boolValue = convertByteArrayToBool(payload, payloadLength);
    } else if(this->type->id == CanbusMessageType::INT->id) {
        this->value.intValue = convertByteArrayToInt(payload, payloadLength);
    } else if(this->type->id == CanbusMessageType::FLOAT->id) {
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

//TODO: check memory leak
String TypedCanbusMessage::getValueToString() {
    Serial.print("TypedCanbusMessage::getValueToString ");
    Serial.print(this->type->name);
    Serial.print(" ");
    Serial.println(getIntValue());
    if(this->type->id == CanbusMessageType::BOOL->id) {
        return this->getBoolValue() ? "true" : "false";
    } else if(this->type->id == CanbusMessageType::INT->id) {
        return String(this->getIntValue());
    } else if(this->type->id == CanbusMessageType::FLOAT->id) {
        return String(this->getFloatValue());
    }
    return "";
};
