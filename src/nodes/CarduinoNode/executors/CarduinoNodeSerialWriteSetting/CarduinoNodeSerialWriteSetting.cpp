#include "CarduinoNodeSerialWriteSetting.h"

CarduinoNodeSerialWriteSetting::CarduinoNodeSerialWriteSetting() : CarduinoNodeExecutorInterface(EV_WRITE_SETTING) {};

void CarduinoNodeSerialWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper("CarduinoNodeSerialWriteSetting::execute");
    node->sendCanbusMessage(message);

    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeWriteSetting WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */

    auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(message->event);
    uint8_t  settingId = std::get<0>(ev->values);
    int32_t  value     = std::get<1>(ev->values);
    
    Setting *setting = (Setting*)Setting::getValueById(settingId);

    if(setting->type->id == CanbusMessageType::BOOL.id) {
        node->putSettingValue(setting, (bool) value); 
    } else if(setting->type->id == CanbusMessageType::INT.id) {
        node->putSettingValue(setting, (int) value);
    } else if(setting->type->id == CanbusMessageType::FLOAT.id) {
        node->putSettingValue(setting, (float) value);
    }
};

bool CarduinoNodeSerialWriteSetting::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
