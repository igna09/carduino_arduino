#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"

class ReadSettingMessage : public TypedCanbusMessage {
    public:
        const Setting *setting;

        ReadSettingMessage(CanbusMessage *canbusMessage);
        ReadSettingMessage(const Setting *setting, int value);
        ReadSettingMessage(const Setting *setting, bool value);
        ReadSettingMessage(const Setting *setting, float value);

        String toSerialHumanString() override;
};
