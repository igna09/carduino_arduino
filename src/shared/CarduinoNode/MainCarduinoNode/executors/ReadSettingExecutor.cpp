#include "ReadSettingExecutor.h"

ReadSettingExecutor::ReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTINGS) {};

void ReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    TypedCanbusMessage otaModeTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::OTA_MODE), node->otaMode);
    SettingMessage otaModeSettingMessage(otaModeTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&otaModeSettingMessage);

    
    TypedCanbusMessage resetTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::RESTART), false);
    SettingMessage resetSettingMessage(resetTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&resetSettingMessage);
};
