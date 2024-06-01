#include "EventMessage.h"

EventMessage::EventMessage(const Event *event, int nodeId) : TypedCanbusMessage(CarduinoNode::generateId(Category::EVENT, *event), nodeId) {
    this->event = event;
    this->nodeId = nodeId;
};

EventMessage::EventMessage(CanbusMessage *canbusMessage) : TypedCanbusMessage(((Event*) Event::getValueById(canbusMessage->messageId))->type, canbusMessage->id, canbusMessage->payload, canbusMessage->payloadLength) {
    this->event = (Event*) Event::getValueById(canbusMessage->messageId);
    this->nodeId = this->getIntValue();
}

String EventMessage::toSerialHumanString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->event->name;
    s += ";";
    s += String(this->nodeId);
    s += ";";
    return s;
};

EventMessage* EventMessage::createSpecializedCopy(CanbusMessage* canbusMessage) {
    return new EventMessage(canbusMessage);
};
