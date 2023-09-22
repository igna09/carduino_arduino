#include "CarstatusExecutor.h"
#include "../../../enums/Setting.h"

CarstatusExecutor::CarstatusExecutor() : CarduinoNodeExecutorInterface(Category::CAR_STATUS) {}

//TODO: check memory leak
void CarstatusExecutor::execute(CarduinoNode *node, CanbusMessage *message) {
    // Setting::AUTO_CLOSE_REARVIEW_MIRRORS;
    Serial.print("CarstatusExecutor::execute ");
    Serial.println(message->messageId);

    {
        const Carstatus *cs = Carstatus::EXTERNAL_TEMPERATURE;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.print(" ");
        Serial.print((unsigned long int)&(cs->type), HEX);
        Serial.println();
        cs = Carstatus::INTERNAL_TEMPERATURE;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.print(" ");
        Serial.print((unsigned long int)&(cs->type), HEX);
        Serial.println();
    }
    Serial.println("---------");
    {
        const CanbusMessageType *cs = CanbusMessageType::BOOL;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->id);
        Serial.print(" ");
        Serial.print((unsigned long int)&cs, HEX);
        Serial.println();
        cs = CanbusMessageType::INT;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->id);
        Serial.print(" ");
        Serial.print((unsigned long int)&cs, HEX);
        Serial.println();
    }
    Serial.println("---------");
    {
        const Setting *cs = Setting::OTA_MODE;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.print(" ");
        Serial.print((unsigned long int)&(cs->type), HEX);
        // Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.println();
        cs = Setting::AUTO_CLOSE_REARVIEW_MIRRORS;
        Serial.print(cs->name);
        Serial.print(" ");
        Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.print(" ");
        Serial.print((unsigned long int)&(cs->type), HEX);
        // Serial.print(cs->type == nullptr ? "true" : "false");
        Serial.println();
    }

    const Carstatus *c = (Carstatus*)Carstatus::getValueById(message->messageId);
    const CanbusMessageType *t = c->type;
    Serial.println(c->name);
    Serial.println(t->name);

    CarstatusMessage *carstatusMessage = new CarstatusMessage(t, message);

    ((MainCarduinoNode*)node)->sendSerialMessage(carstatusMessage);

    delete carstatusMessage;
}
