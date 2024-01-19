#pragma once

#include <Arduino.h>

#include <ESP8266WiFi.h>

#include "mcp_can.h"
#include <SPI.h>
#include <TaskSchedulerDeclarations.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include "../../utils.h"
#include "../../CanbusMessage/CanbusMessage.h"
#include "../../CanbusMessage/HeartbeatMessage/HeartbeatMessage.h"
#include "../../SharedDefinitions.h"
#include "callbacks/SendHearbeatCallback.h"
#include "../../Logger/Logger.h"

/**
 * Send message to android --> Category;payload;
 * READ_SETTINGS;auto_close_rearview_mirrors-true;
 * CAR_STATUS;INTERNAL_LUMINANCE-2700;
 * Receive message from android --> Category;paylod;
 * READ_SETTINGS;get;
 * WRITE_SETTING;auto_close_rearview_mirrors-false;
 * 
 * This node has got a canbus interface and a wifi AP to update software
*/

const char FALLBACK_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset="UTF-8">
</head>
<body>
<p><h1>File Upload</h1></p>
<form method="POST" action="/file-upload" enctype="multipart/form-data">
    <p><input type="file" name="file" multiple/></p>
    <input type="submit" name="upload" value="Upload" title="Upload File">
</form>
<p>Some files are missing, upload ALL webapp files (favicon.ico.gz index.html.gz main.js.gz polyfills.js.gz styles.css.gz)</p>
<p>You will be redirect as soon as the upload ends (if you uploaded ALL files)</p>
</body>
</html>
)rawliteral";

class WriteSetting; // forward declaration to avoid circular dependency
class Executors; // forward declaration to avoid circular dependency
class CarduinoNode : public Logger {
    private:
        String fallbackPageProcessor(const String& var);
        bool existsAllFiles();
        int requestsCounter;
        void setupServerWebapp();
        void setupServerFallback();
        bool originalLogOnWebserver;
        bool originalLogOnSerial;

    public:
        uint8_t id;
        MCP_CAN *can;
        AsyncWebServer *server;
        String ssid;
        String password;
        bool otaMode;
        int interruptPin;
        Executors *canExecutors;
        bool initializedCan;
        Scheduler *scheduler;
        Task *temperatureTask;

        CarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password, bool logOnServer, bool logOnSerial);

        void loop();
        void manageReceivedCanbusMessage(CanbusMessage *message);
        void sendByteCanbus(uint16_t messageId, int len, uint8_t buf[]);
        void sendCanbusMessage(CanbusMessage *message);
        bool availableCanbusMessages();
        void otaStartup();
        void otaShutdown();
        void restart();
        void sendHeartbeat();

        static uint16_t generateId(const Category category, const Enum messageEnum);
        static uint16_t generateId(const Category category, uint8_t messageId);
};
