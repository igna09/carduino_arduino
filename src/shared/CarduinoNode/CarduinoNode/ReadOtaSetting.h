#pragma once

#include <Arduino.h>
#include "shared/CanbusMessage/CanbusMessage.h"
#include "CarduinoNode.h"
#include "shared/executors/CarduinoNodeExecutorInterface.h"
#include "shared/enums/Setting.h"
#include "shared/CanbusMessage/SettingMessage/SettingMessage.h"
#include "shared/enums/Category.h"

class ReadOtaSetting : public CarduinoNodeExecutorInterface {
    public:
        ReadOtaSetting() : CarduinoNodeExecutorInterface(&Category::READ_SETTINGS) {};

        void execute(CarduinoNode *node, CanbusMessage *message) {
            TypedCanbusMessage typedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::OTA_MODE), node->otaMode);
            node->sendCanbusMessage(&typedCanbusMessage);
        };
};
