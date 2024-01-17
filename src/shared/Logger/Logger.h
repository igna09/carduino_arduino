#pragma once

#include <Arduino.h>

class Logger {
    private:
        bool _logOnServer;
        bool _logOnSerial;

    public:
        Logger(ESP8266WebServer *server);

        void logOnServer(const char c[]);

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
