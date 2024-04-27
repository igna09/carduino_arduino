#include "MediaControlExecutor.h"

MediaControlExecutor::MediaControlExecutor() : CarduinoNodeExecutorInterface(&Category::MEDIA_CONTROL) {};

void MediaControlExecutor::execute(CarduinoNode *node, CanbusMessage *message){
    MediaControlMessage mediaControlMessage = MediaControlMessage((MediaControl*)MediaControl::getValueById(message->messageId));

    if(mediaControlMessage.id == MediaControl::LONG_PRESS.id) {
        ((MainCarduinoNode*)node)->sendSerialMessage(&mediaControlMessage);
    } else if(mediaControlMessage.id != MediaControl::LONG_PRESS.id) { // TODO: move to new node canbus executor
        ((MainCarduinoNode*)node)->executeSwcCommand((MediaControl*)mediaControlMessage.mediaControl);
    }
};
