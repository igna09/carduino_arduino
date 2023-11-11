#include "MediaControlExecutor.h"

MediaControlExecutor::MediaControlExecutor() : CarduinoNodeExecutorInterface(&Category::MEDIA_CONTROL) {};

void MediaControlExecutor::execute(CarduinoNode *node, CanbusMessage *message){
            //Serial.println("received canbus media message");
    //MediaControlMessage mediaControlMessage = MediaControlMessage((MediaControl*)MediaControl::getValueById(message->messageId));
    Serial.println(((MediaControl*)MediaControl::getValueById(message->messageId))->name);
    //((MainCarduinoNode*)node)->sendSerialMessage(&mediaControlMessage);
};
