#pragma once

#include <Arduino.h>
#include "../../enums/CanbusMessageType.h"
#include "../../SharedDefinitions.h"

class ValueTypedBase
{
    public:
        const CanbusMessageType *type;
        ValueType value;
        
        ValueTypedBase(){};
        ValueTypedBase(int v){
            this->type = &CanbusMessageType::INT;
            this->value.intValue = v;
        };
        ValueTypedBase(float v){
            this->type = &CanbusMessageType::FLOAT;
            this->value.floatValue = v;
        };
        ValueTypedBase(bool v){
            this->type = &CanbusMessageType::BOOL;
            this->value.boolValue = v;
        };
        virtual ~ValueTypedBase() = default;

        int getIntValue() {
            return this->value.intValue;
        };

        float getFloatValue() {
            return this->value.floatValue;
        };

        bool getBoolValue() {
            return this->value.boolValue;
        };

        String getValueToString() {
            if(this->type->id == CanbusMessageType::BOOL.id) {
                return this->getBoolValue() ? "true" : "false";
            } else if(this->type->id == CanbusMessageType::INT.id) {
                return String(this->getIntValue());
            } else if(this->type->id == CanbusMessageType::FLOAT.id) {
                return String(this->getFloatValue());
            }
            return "";
        };
};
