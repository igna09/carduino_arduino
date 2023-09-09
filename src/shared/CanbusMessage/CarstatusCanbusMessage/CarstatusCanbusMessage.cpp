#include "CarStatusCanbusMessage.h"

template<class T>
CarstatusCanbusMessage<T>::CarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength, std::function<T(uint8_t*, int)> convertByteArrayToTypeFunction): CanbusMessage(id, payload, payloadLength) {
    this->carstatus = static_cast<Carstatus>(payload[0]);

    uint8_t sub[this->payloadLength - 1];
    int m;
    pickSubarray(this->payload, sub, m, 1, this->payloadLength);
    this->convertByteArrayToTypeFunction = convertByteArrayToTypeFunction;
    this->value = this->convertByteArrayToTypeFunction(sub, this->payloadLength - 1);
};

template<class T>
CarstatusCanbusMessage<T>::~CarstatusCanbusMessage() {};

template<class T>
String CarstatusCanbusMessage<T>::getPayloadString() {
    String s = "";
    s += CARSTATUS_STRING[carstatus];
    s += "-";
    s += value;
    return s;
};
