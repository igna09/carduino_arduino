#include "Logger.h"

Logger::Logger() {};

void Logger::setupLogger(AsyncWebServer *server, bool logOnServer, bool logOnSerial) {
    this->_logOnSerial = logOnSerial;
    this->_logOnServer = logOnServer;
    if(this->_logOnServer) {
        if(this->_webSocket != nullptr) {
            webServer->removeHandler(this->_webSocket);
            delete this->_webSocket;
        }
        this->_webSocket = new AsyncWebSocket("/ws");
        this->_webServer = server;
        this->_webSocket->onEvent([&](AsyncWebSocket * webSocket, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
            this->onWebSocketEvent(client, type, arg, data, len);
        });
        this->_webServer->addHandler(this->_webSocket);
    }
};

void Logger::onWebSocketEvent(AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  switch(type) {
    case WS_EVT_DISCONNECT:
        // Serial.printf("ws[%s][%u] disconnect: %u\n", this->_webSocket->url(), client->id());
        break;
    case WS_EVT_CONNECT:
        // Serial.printf("ws[%s][%u] connect\n", this->_webSocket->url(), client->id());
        break;
    case WS_EVT_PONG:
        // Serial.printf("ws[%s][%u] pong[%u]: %s\n", this->_webSocket->url(), client->id(), len, (len)?(char*)data:"");
        break;
    case WS_EVT_DATA:
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            // printf("ws[%s][%u] %s-message[%llu]: ", this->_webSocket->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
            if(info->opcode == WS_TEXT){
                data[len] = 0;
                // printf("%s\n", (char*)data);
            } else {
                for(size_t i=0; i < info->len; i++){
                // printf("%02x ", data[i]);
                }
                // printf("\n");
            }
            if(info->opcode == WS_TEXT)
                client->text("I got your text message");
            else
                client->binary("I got your binary message");
        } else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if(info->index == 0){
                if(info->num == 0){
                    // printf("ws[%s][%u] %s-message start\n", this->_webSocket->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                }
                // printf("ws[%s][%u] frame[%u] start[%llu]\n", this->_webSocket->url(), client->id(), info->num, info->len);
            }

            // printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", this->_webSocket->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
            if(info->message_opcode == WS_TEXT){
                data[len] = 0;
                // printf("%s\n", (char*)data);
            } else {
                for(size_t i=0; i < len; i++){
                    // printf("%02x ", data[i]);
                }
                // printf("\n");
            }

            if((info->index + len) == info->len){
                // printf("ws[%s][%u] frame[%u] end[%llu]\n", this->_webSocket->url(), client->id(), info->num, info->len);
                if(info->final){
                    // printf("ws[%s][%u] %s-message end\n", this->_webSocket->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                    if(info->message_opcode == WS_TEXT)
                        client->text("I got your text message");
                    else
                        client->binary("I got your binary message");
                }
            }
        }
        break;
  }
}

void Logger::logOnServer(String message) {
    this->_webSocket->textAll(message);
}

void Logger::printlnWrapper(const String &s) {
    if(_logOnSerial) Serial.println(s);
    if(_logOnServer) logOnServer(s + "\n");
}

void Logger::printlnWrapper(const char c[]) {
    if(_logOnSerial) Serial.println(c);
    if(_logOnServer) logOnServer(String(c) + "\n");
}

void Logger::printlnWrapper(char c) {
    if(_logOnSerial) Serial.println(c);
}

void Logger::printlnWrapper(unsigned char b, int base) {
    if(_logOnSerial) Serial.println(b, base);
}

void Logger::printlnWrapper(int num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned int num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(long long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long long num, int base) {
    if(_logOnSerial) Serial.println(num, base);
}

void Logger::printlnWrapper(double num, int digits) {
    if(_logOnSerial) Serial.println(num, digits);
}

void Logger::printWrapper(const String &s) {
    if(_logOnSerial) Serial.print(s);
    if(_logOnServer) logOnServer(s);
}

void Logger::printWrapper(const char c[]) {
    if(_logOnSerial) Serial.print(c);
    if(_logOnServer) logOnServer(String(c));
}

void Logger::printWrapper(char c) {
    if(_logOnSerial) Serial.print(c);
}

void Logger::printWrapper(unsigned char b, int base) {
    if(_logOnSerial) Serial.print(b, base);
}

void Logger::printWrapper(int num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned int num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(long long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(unsigned long long num, int base) {
    if(_logOnSerial) Serial.print(num, base);
}

void Logger::printWrapper(double num, int digits) {
    if(_logOnSerial) Serial.print(num, digits);
}
