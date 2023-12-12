#include "ReadSettingExecutor.h"

ReadSettingExecutor::ReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTINGS) {};

void ReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    TypedCanbusMessage typedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::OTA_MODE), node->otaMode);
    SettingMessage settingMessage(typedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&settingMessage);
};
