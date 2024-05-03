#include "ReadSettingExecutor.h"

ReadSettingExecutor::ReadSettingExecutor() : CarduinoNodeExecutorInterface(&Category::READ_SETTING) {};

void ReadSettingExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    TypedCanbusMessage otaModeTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::OTA_MODE), node->otaMode);
    ReadSettingMessage otaModeSettingMessage(otaModeTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&otaModeSettingMessage);

    
    TypedCanbusMessage resetTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::RESTART), false);
    ReadSettingMessage resetSettingMessage(resetTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&resetSettingMessage);

    /**
     * TODO: move to new node, should add method to send read setting on canbus? yes
    */
    TypedCanbusMessage swcBindingTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::SWC_PAIR), false);
    ReadSettingMessage swcBindingSettingMessage(swcBindingTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&swcBindingSettingMessage);
};
