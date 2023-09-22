#include "WriteSetting.h"

WriteSetting::WriteSetting(const Category *categoryFilter) : CarduinoNodeExecutorInterface(categoryFilter) {}

void WriteSetting::execute(CarduinoNode *node, CanbusMessage message) {
    SettingMessage settingMessage(message);
    
    if(settingMessage.setting->id == Setting::OTA_MODE->id) {
        node->otaMode = settingMessage.getBoolValue();
    }

    // ((MainCarduinoNode*)node)->sendSerialMessage(message);
};
