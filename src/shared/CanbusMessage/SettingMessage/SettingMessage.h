#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class SettingMessage : public TypedCanbusMessage {
    public:
        const Setting *setting;

        SettingMessage(CanbusMessage *canbusMessage);
        SettingMessage(const Setting *setting, int value);
        SettingMessage(const Setting *setting, bool value);
        SettingMessage(const Setting *setting, float value);

        static SettingMessage* createSpecializedCopy(CanbusMessage *canbusMessage);
        String toSerialHumanString() override;
};
