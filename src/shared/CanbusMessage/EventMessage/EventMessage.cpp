#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, int senderReceiverId) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), senderReceiverId) {
    this->event = event;
    this->senderReceiverId = senderReceiverId;
};

EventMessage::EventMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(((Event*) Event::getValueById(canbusMessage->messageId))->type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->event = (Event*) Event::getValueById(canbusMessage->messageId);
    this->senderReceiverId = this->getIntValue();
}

String EventMessage::toSerialHumanString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->event->name;
    s += ";";
    s += String(this->senderReceiverId);
    s += ";";
    return s;
};

EventMessage* EventMessage::createSpecializedCopy(CanbusMessage* canbusMessage) {
    return new EventMessage(canbusMessage);
};
