#include "WriteSetting.h"

void WriteSetting::execute(CarduinoNode *node, CanbusMessage message) {
    SettingMessage settingMessage(message);
    
    if((Enum)*settingMessage.setting == (Enum)Setting::OTA_MODE) {
        node->otaMode = settingMessage.getBoolValue();
    }
};
