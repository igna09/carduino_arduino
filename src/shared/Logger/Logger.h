#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "shared/FSBase/FSBase.h"

// class FSBase;
class Logger {
    private:
        static const char PROGMEM INDEX_HTML[];

    public:
        AsyncWebSocket *_webSocket;
        bool _logOnServer;
        bool _logOnSerial;
        bool _originalLogOnWebserver;
        bool _originalLogOnSerial;
        FSBase* _fsBase;

        Logger(FSBase* fsBase, bool logOnSerial);
        void setupLogger(AsyncWebServer *server, bool logOnServer, bool logOnSerial);

        void onWebSocketEvent(AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

        void logOnServer(String message);
        void logOnFile(String message);

        void printlnWrapper(const String &s, bool logOnFile = false);
        void printlnWrapper(const char c[], bool logOnFile = false);
};
