#include "MediaControlMessage.h"

MediaControlMessage::MediaControlMessage(const MediaControl *mediaControl) : TypedCanbusMessage(CarduinoNode::generateId(Category::MEDIA_CONTROL, *mediaControl), 0) {
    this->mediaControl = mediaControl;
};

String MediaControlMessage::toSerialString() {
    String s = "";
    s += this->category->name;
    s += ";";
    s += this->mediaControl->name;
    s += ";";
    s += "0";
    s += ";";
    return s;
};
