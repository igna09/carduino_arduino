#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, int senderId) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), senderId) {
    this->event = event;
    this->senderId = senderId;
};

EventMessage::EventMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(((Event*) Event::getValueById(canbusMessage->messageId))->type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->event = (Event*) Event::getValueById(canbusMessage->messageId);
    this->senderId = this->getIntValue();
}

String EventMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->event->name;
    s += ";";
    s += String(this->senderId);
    s += ";";
    return s;
};

EventMessage* EventMessage::createSpecializedCopy(CanbusMessage* canbusMessage) {
    return new EventMessage(canbusMessage);
};
