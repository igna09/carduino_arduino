#include "MainNodeSerialGetSettings.h"

MainNodeSerialGetSettings::MainNodeSerialGetSettings() : CarduinoNodeExecutorInterface(&Category::GET_SETTINGS) {};

void MainNodeSerialGetSettings::execute(CarduinoNode *node, CanbusMessage *message) {
    node->sendCanbusMessage(message);

    TypedCanbusMessage otaModeTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::OTA_MODE), node->otaMode);
    ReadSettingMessage otaModeSettingMessage(&otaModeTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&otaModeSettingMessage);

    
    TypedCanbusMessage resetTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::RESTART), false);
    ReadSettingMessage resetSettingMessage(&resetTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&resetSettingMessage);

    /**
     * TODO: move swc to new node
    */
    TypedCanbusMessage swcBindingTypedCanbusMessage = TypedCanbusMessage(node->generateId(Category::READ_SETTING, Setting::SWC_PAIR), false);
    ReadSettingMessage swcBindingSettingMessage(&swcBindingTypedCanbusMessage);
    ((MainCarduinoNode*)node)->sendSerialMessage(&swcBindingSettingMessage);
};
