#include "MediaControlCanExecutor.h"

MediaControlCanExecutor::MediaControlCanExecutor() : CarduinoNodeExecutorInterface(&Category::MEDIA_CONTROL) {};

void MediaControlCanExecutor::execute(CarduinoNode *node, CanbusMessage *message){
    MediaControl *mediaControl = (MediaControl*)MediaControl::getValueById(message->messageId);
    if(mediaControl->id == MediaControl::LONG_PRESS.id) {
        MediaControlMessage *mediaControlMessage = new MediaControlMessage(mediaControl);
        ((MainCarduinoNode*)node)->sendSerialMessage(mediaControlMessage);
        delete mediaControlMessage;
    } /*else if(mediaControl->id != MediaControl::LONG_PRESS.id) { // TODO: move to new node canbus executor
        ((MainCarduinoNode*)node)->executeSwcCommand(mediaControl);
    }*/
};

bool MediaControlCanExecutor::canExecute(CarduinoNode *node, CanbusMessage *message) {
    return node->isEnabled;
}
