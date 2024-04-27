#include "ReadSettingExecutor.h"

ReadSettingExecutor::ReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTINGS) {};

void ReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    TypedCanbusMessage otaModeTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::OTA_MODE), node->otaMode);
    SettingMessage otaModeSettingMessage(otaModeTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&otaModeSettingMessage);

    
    TypedCanbusMessage resetTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::RESTART), false);
    SettingMessage resetSettingMessage(resetTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&resetSettingMessage);

    /**
     * TODO: move to new node, should add method to send read setting on canbus? yes
    */
    TypedCanbusMessage swcBindingTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTINGS, Setting::SWC_PAIR), false);
    SettingMessage swcBindingSettingMessage(swcBindingTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&swcBindingSettingMessage);
};
