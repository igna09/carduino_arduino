#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class Logger {
    private:
        static const char PROGMEM INDEX_HTML[];

    public:
        AsyncWebSocket *_webSocket;
        bool _logOnServer;
        bool _logOnSerial;
        bool _originalLogOnWebserver;
        bool _originalLogOnSerial;
        Logger();
        void setupLogger(AsyncWebServer *server, bool logOnServer, bool logOnSerial);

        void onWebSocketEvent(AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

        void logOnServer(String message);

        void printlnWrapper(const String &s);
        void printlnWrapper(const char c[]);
        void printlnWrapper(char c);
        void printlnWrapper(unsigned char b, int base);
        void printlnWrapper(int num, int base);
        void printlnWrapper(unsigned int num, int base);
        void printlnWrapper(long num, int base);
        void printlnWrapper(unsigned long num, int base);
        void printlnWrapper(long long num, int base);
        void printlnWrapper(unsigned long long num, int base);
        void printlnWrapper(double num, int digits);
        void printWrapper(const String &s);
        void printWrapper(const char c[]);
        void printWrapper(char c);
        void printWrapper(unsigned char b, int base);
        void printWrapper(int num, int base);
        void printWrapper(unsigned int num, int base);
        void printWrapper(long num, int base);
        void printWrapper(unsigned long num, int base);
        void printWrapper(long long num, int base);
        void printWrapper(unsigned long long num, int base);
        void printWrapper(double num, int digits);
};
