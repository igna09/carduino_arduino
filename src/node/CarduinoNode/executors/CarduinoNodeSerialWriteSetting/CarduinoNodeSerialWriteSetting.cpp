#include "CarduinoNodeSerialWriteSetting.h"
// #include "EventMulti.h"

// CarduinoNodeSerialWriteSetting::CarduinoNodeSerialWriteSetting() : CarduinoNodeExecutorInterface({EV_ENABLE, EV_DISABLE}) {};
CarduinoNodeSerialWriteSetting::CarduinoNodeSerialWriteSetting(): CarduinoNodeExecutorInterface(EV_WRITE_SETTING) {}

void CarduinoNodeSerialWriteSetting::execute(CarduinoNode *node, Message *message) {
    NLOGD("CarduinoNodeSerialWriteSetting::execute");

    // sendMessage prende una const Message&: internamente chiama solo
    // m.toCanFrame()/m.canId(), entrambi const, quindi non c'è bisogno di
    // copiare né spostare il Message (che tra l'altro non è copiabile).
    // *message resta di proprietà di chi l'ha creato in rxTaskEntry.
    node->sendMessage(*message);

    /**
     * QUESTA LOGICA DEVE ESSERE REPLICATA QUI PERCHÉ L'EQUIVALENTE "WriteSetting"
     * SU CANBUS VERREBBE INVOCATO SOLO PER MESSAGGI RICEVUTI DAL BUS (QUI SIAMO SU SERIALE)
     */

    auto* ev = static_cast<EventMulti<uint8_t, int32_t>*>(message->event);
    uint8_t settingId = std::get<0>(ev->values);
    int32_t value     = std::get<1>(ev->values);

    Setting *setting = (Setting*)Setting::getValueById(settingId);

    if (setting->type->id == MessageType::BOOL.id) {
        node->putSettingValue(setting, (bool) value);
    } else if (setting->type->id == MessageType::INT.id) {
        node->putSettingValue(setting, (int) value);
    } else if (setting->type->id == MessageType::FLOAT.id) {
        node->putSettingValue(setting, (float) value);
    }
}