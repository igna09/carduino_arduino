#include "CarduinoNodeWriteSetting.h"

CarduinoNodeWriteSetting::CarduinoNodeWriteSetting() : CarduinoNodeExecutorInterface(&EventEnum::WRITE_SETTING) {};

void CarduinoNodeWriteSetting::execute(CarduinoNode *node, CanbusMessage *message) {
    // Iteriamo su tutti i possibili valori trasportati dall'evento
    EventEnum *eventEnum = (EventEnum*)EventEnum::getValueById(message->eventId);
    
    // 1. Estraiamo l'ID del setting (Indice 0 del payload)
    uint8_t settingIdOffset = 0;
    uint8_t settingId = message->unpackValue<uint8_t>(settingIdOffset);

    // 2. Estraiamo il tipo di dato reale nascosto nel contenitore (Indice 1 del payload)
    uint8_t typeOffset = settingIdOffset + eventEnum->types[0].size; // L'offset del tipo viene calcolato in base alla dimensione del primo campo (settingId)
    uint8_t typeId = message->unpackValue<uint8_t>(typeOffset);
    DataTypeEnum* realType = (DataTypeEnum*)DataTypeEnum::getValueById(typeId);

    // 3. Calcoliamo l'offset di partenza del valore (Indice 2 del payload)
    uint8_t valueOffset = typeOffset + eventEnum->types[1].size;

    
    Setting *setting = (Setting*)Setting::getValueById(settingId);

    // 4. Smistiamo il valore in base al tipo reale dichiarato nel payload stesso
    if(realType->id == DataTypeEnum::BOOL.id) {
        // Un bool occupa 1 byte all'inizio del contenitore da 4 byte
        bool val = message->unpackValue<bool>(valueOffset);
        node->putSettingValue(setting, val); 
    } else if(realType->id == DataTypeEnum::INT32.id || realType->id == DataTypeEnum::UINT16.id) {
        int val = message->unpackValue<int32_t>(valueOffset);
        node->putSettingValue(setting, val);
    } else if(realType->id == DataTypeEnum::FLOAT.id) {
        // I 4 byte vengono reinterpretati correttamente come float binario
        float val = message->unpackValue<float>(valueOffset);
        node->putSettingValue(setting, val);
    }
};

bool CarduinoNodeWriteSetting::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return true;
}
