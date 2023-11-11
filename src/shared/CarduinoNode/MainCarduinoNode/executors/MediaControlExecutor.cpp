#include "MediaControlExecutor.h"

MediaControlExecutor::MediaControlExecutor() : CarduinoNodeExecutorInterface(&Category::MEDIA_CONTROL) {};

void MediaControlExecutor::execute(CarduinoNode *node, CanbusMessage *message){
    MediaControlMessage mediaControlMessage = MediaControlMessage((MediaControl*)MediaControl::getValueById(message->messageId));
    ((MainCarduinoNode*)node)->sendSerialMessage(&mediaControlMessage);
};
