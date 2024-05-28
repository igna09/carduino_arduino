#pragma once

#include <Arduino.h>
#include "../utils.h"

/**
 * message structure:
 * id -> 11 bit = 3 bit for category (CAR_STATUS, GET_SETTING, ecc.) + 1 byte for message id
 * payload -> 8 byte
*/

class CanbusMessage {
    public:
        uint16_t id;
        uint8_t *payload;
        uint8_t payloadLength;
        uint8_t categoryId;
        uint8_t messageId;

        CanbusMessage();
        CanbusMessage(CanbusMessage &m);
        CanbusMessage(uint16_t id, uint8_t *payload, uint8_t payloadLength);
        virtual ~CanbusMessage();

        static CanbusMessage* createSpecializedCopy(CanbusMessage* message) {
            return new CanbusMessage(message->id, message->payload, message->payloadLength);
        };

        virtual String toSerialHumanString() {
            String returnValue = "";
            returnValue += categoryId;
            returnValue += ";";
            returnValue += messageId;
            returnValue += ";";
            for(int i=0; i < payloadLength; i++){
                returnValue += payload[i];
                if(i != (payloadLength - 1)) {
                    returnValue += " ";
                }
            }
            returnValue += ";";
            return returnValue;
        };

        virtual String toSerialString() {
            String returnValue = "";
            returnValue += categoryId;
            returnValue += ";";
            returnValue += messageId;
            returnValue += ";";
            for(int i=0; i < payloadLength; i++){
                returnValue += payload[i];
                if(i != (payloadLength - 1)) {
                    returnValue += " ";
                }
            }
            returnValue += ";";
            return returnValue;
        };
};
