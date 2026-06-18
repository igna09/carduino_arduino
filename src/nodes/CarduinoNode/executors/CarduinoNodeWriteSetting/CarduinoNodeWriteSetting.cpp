#include "CarduinoNodeWriteSetting.h"

CarduinoNodeWriteSetting::CarduinoNodeWriteSetting() : CarduinoNodeExecutorInterface(EV_WRITE_SETTING) {};

void CarduinoNodeWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(message->event);
    uint8_t  settingId = std::get<0>(ev->values);  // 1
    int32_t  value     = std::get<1>(ev->values);
    
    Setting *setting = (Setting*)Setting::getValueById(settingId);

    // 4. Smistiamo il valore in base al tipo reale dichiarato nel payload stesso
    if(setting->type->id == CanbusMessageType::BOOL.id) {
        node->putSettingValue(setting, (bool) value); 
    } else if(setting->type->id == CanbusMessageType::INT.id) {
        node->putSettingValue(setting, (int) value);
    } else if(setting->type->id == CanbusMessageType::FLOAT.id) {
        node->putSettingValue(setting, (float) value);
    }
};

bool CarduinoNodeWriteSetting::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
