#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "../TypedCanbusMessage/TypedCanbusMessage.h"

class SettingMessage : public TypedCanbusMessage {
    public:
        const Setting *setting;

        SettingMessage(CanbusMessage canbusMessage);
};
