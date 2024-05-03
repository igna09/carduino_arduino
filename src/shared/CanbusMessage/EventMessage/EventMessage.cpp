#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, ) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), 0) { //TODO: replace 0 with id?
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
