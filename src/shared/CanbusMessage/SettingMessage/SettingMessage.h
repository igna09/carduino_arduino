#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "shared/CanbusMessage/TypedCanbusMessage/TypedCanbusMessage.h"
#include "nodes/CarduinoNode/CarduinoNode.h"

class SettingMessage : public TypedCanbusMessage {
    public:
        const Setting *setting;

        SettingMessage(CanbusMessage *canbusMessage);
        SettingMessage(const Setting *setting, bool isRead, int value);
        SettingMessage(const Setting *setting, bool isRead, bool value);
        SettingMessage(const Setting *setting, bool isRead, float value);

        static SettingMessage* createSpecializedCopy(CanbusMessage *canbusMessage);
        String toSerialHumanString() override;
};
