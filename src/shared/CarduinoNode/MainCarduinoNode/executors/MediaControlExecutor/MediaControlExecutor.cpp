#include "MediaControlExecutor.h"

MediaControlExecutor::MediaControlExecutor() : CarduinoNodeExecutorInterface(&Category::MEDIA_CONTROL) {};

void MediaControlExecutor::execute(CarduinoNode *node, CanbusMessage *message){
    MediaControl *mediaControl = (MediaControl*)MediaControl::getValueById(message->messageId);
    if(mediaControl->id == MediaControl::LONG_PRESS.id) {
        MediaControlMessage mediaControlMessage = MediaControlMessage(mediaControl);
        ((MainCarduinoNode*)node)->sendSerialMessage(&mediaControlMessage);
    } else if(mediaControl->id != MediaControl::LONG_PRESS.id) { // TODO: move to new node canbus executor
        ((MainCarduinoNode*)node)->executeSwcCommand(mediaControl);
    }
};
