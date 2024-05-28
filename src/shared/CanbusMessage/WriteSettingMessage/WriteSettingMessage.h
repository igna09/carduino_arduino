#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class WriteSettingMessage : public TypedCanbusMessage {
    public:
        const Setting *setting;

        WriteSettingMessage(CanbusMessage canbusMessage);
        WriteSettingMessage(const Setting *setting, int value);
        WriteSettingMessage(const Setting *setting, bool value);
        WriteSettingMessage(const Setting *setting, float value);

        String toSerialHumanString() override;
};
