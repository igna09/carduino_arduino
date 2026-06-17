#include "CarduinoNodeSerialWriteSetting.h"

CarduinoNodeSerialWriteSetting::CarduinoNodeSerialWriteSetting() : CarduinoNodeExecutorInterface(&EventEnum::WRITE_SETTING) {};

void CarduinoNodeSerialWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    node->printlnWrapper("CarduinoNodeSerialWriteSetting::execute");
    node->sendCanbusMessage(message);

    /**
     * THIS LOGIC HAS TO BE REPLICATED HERE BECAUSE CarduinoNodeWriteSetting WILL BE CALLED ONLY ON CANBUS MESSAGES (HERE WE ARE ON SERIAL)
    */
    // Iteriamo su tutti i possibili valori trasportati dall'evento
    EventEnum *eventEnum = (EventEnum*)EventEnum::getValueById(message->eventId);
    
    // 1. Estraiamo l'ID del setting (Indice 0 del payload)
    uint8_t settingIdOffset = 0;
    uint8_t settingId = message->unpackValue<uint8_t>(settingIdOffset);

    // 2. Calcoliamo l'offset di partenza del valore (Indice 1 del payload)
    uint8_t valueOffset = settingIdOffset + eventEnum->types[0].size;
    
    Setting *setting = (Setting*)Setting::getValueById(settingId);

    node->printlnWrapper("setting type id " + String(setting->type->id));
    node->printlnWrapper("valueOffset " + String(valueOffset));

    // 3. Smistiamo il valore in base al tipo reale dichiarato nel payload stesso
    if(setting->type->id == CanbusMessageType::BOOL.id) {
        bool val = message->unpackValue<bool>(valueOffset);
        node->putSettingValue(setting, val); 
    } else if(setting->type->id == CanbusMessageType::INT.id) {
        int val = message->unpackValue<int32_t>(valueOffset);
        node->putSettingValue(setting, val);
    } else if(setting->type->id == CanbusMessageType::FLOAT.id) {
        float val = message->unpackValue<float>(valueOffset);
        node->putSettingValue(setting, val);
    }
};

bool CarduinoNodeSerialWriteSetting::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
