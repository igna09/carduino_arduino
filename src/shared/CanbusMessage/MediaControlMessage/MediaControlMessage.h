#pragma once

#include <Arduino.h>
#include "../CanbusMessage.h"
#include "../../enums/Setting.h"
#include "../TypedCanbusMessage/TypedCanbusMessage.h"
#include "shared/enums/MediaControl.h"

class MediaControlMessage : public TypedCanbusMessage {
    public:
        const MediaControl *mediaControl;

        MediaControlMessage(const MediaControl*);
        String toSerialString();
};
