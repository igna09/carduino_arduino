#include "CarstatusCanbusMessage.h"

template<class T>
CarstatusCanbusMessage<T>::CarstatusCanbusMessage() : CanbusMessage() {};

template<class T>
CarstatusCanbusMessage<T>::CarstatusCanbusMessage(unsigned long id, uint8_t *payload, uint8_t payloadLength, std::function<T(uint8_t[], int)> convertByteArrayToTypeFunction): CanbusMessage(id, payload, payloadLength) {
    this->carstatus = Carstatus::getValueById(this->messageId);
    this->category = Category::getValueById(this->categoryId);

    this->convertByteArrayToTypeFunction = convertByteArrayToTypeFunction;
    this->value = this->convertByteArrayToTypeFunction(this->payload, this->payloadLength);
};

template<class T>
String CarstatusCanbusMessage<T>::toSerialString() {
    return "";
};

template class CarstatusCanbusMessage<int>;
template class CarstatusCanbusMessage<String>;
template class CarstatusCanbusMessage<float>;
template class CarstatusCanbusMessage<bool>;