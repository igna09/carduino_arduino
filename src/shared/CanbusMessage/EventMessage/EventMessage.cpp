#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, int senderId) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), senderId) {
    this->event = event;
};

EventMessage::EventMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(((Event*) Event::getValueById(canbusMessage->messageId))->type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->event = (Event*) Event::getValueById(canbusMessage->messageId);
}

String EventMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->event->name;
    s += ";";
    s += "0";
    s += ";";
    return s;
};
