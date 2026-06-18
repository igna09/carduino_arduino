#pragma once

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#include "shared/FSBase/FSBase.h"

#define LOGGER_HISTORY_SIZE 20

class Logger {
    public:
        AsyncWebSocket *_webSocket;
        bool _logOnServer;
        bool _logOnSerial;
        bool _originalLogOnWebserver;
        bool _originalLogOnSerial;
        FSBase* _fsBase;

        String _history[LOGGER_HISTORY_SIZE];
        uint8_t _historyIdx = 0;
        bool _historyFull = false;

        Logger(FSBase* fsBase, bool logOnSerial);
        void setupLogger(AsyncWebServer *server, bool logOnServer, bool logOnSerial);

        void setLogOnServer(bool logOnServer);
        void _addLogToHistory(const String &s);
        void onWebSocketEvent(AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);

        void logOnServer(String message);
        void logOnFile(String message);

        void printlnWrapper(const String &s, bool logOnFile = false);
        void printlnWrapper(const char c[], bool logOnFile = false);

        String getFormattedTimestamp(unsigned long nowMillis);
};
