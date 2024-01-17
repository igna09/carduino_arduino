#include "Logger.h"

Logger::Logger(ESP8266WebServer *server, bool logOnServer, bool logOnSerial) {
    this->_logOnSerial = logOnSerial;
    this->_logOnServer = logOnServer;
    if(this->_logOnServer) {
        server->on("/logger", HTTP_GET, [&](){
            //   _server->send_P(200, PSTR("text/html"), serverIndex);
            // TODO: send webapp homepage
        });

        server->on("/logger/logs", HTTP_GET, [&](){
            //   _server->send_P(200, PSTR("text/html"), serverIndex);
            // TODO: send list of logs
        });
    }
};

void Logger::printlnWrapper(const String &s) {
    if(_logOnSerial) Serial.println(s);
    if(_logOnServer) logOnServer(s);
}

void Logger::printlnWrapper(const char c[]) {
    Serial.println(c);
}

void Logger::printlnWrapper(char c) {
    Serial.println(c);
}

void Logger::printlnWrapper(unsigned char b, int base) {
    Serial.println(b, base);
}

void Logger::printlnWrapper(int num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned int num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(long num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(long long num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(unsigned long long num, int base) {
    Serial.println(num, base);
}

void Logger::printlnWrapper(double num, int digits) {
    Serial.println(num, digits);
}

void Logger::printWrapper(const String &s) {
    Serial.println(s);
}

void Logger::printWrapper(const char c[]) {
    Serial.println(c);
}

void Logger::printWrapper(char c) {
    Serial.println(c);
}

void Logger::printWrapper(unsigned char b, int base) {
    Serial.println(b, base);
}

void Logger::printWrapper(int num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(unsigned int num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(long num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(unsigned long num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(long long num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(unsigned long long num, int base) {
    Serial.println(num, base);
}

void Logger::printWrapper(double num, int digits) {
    Serial.println(num, digits);
}
