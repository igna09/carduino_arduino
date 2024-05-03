#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, int senderId) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), senderId) {
    this->event = event;
};

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
