#pragma once

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "mcp_can.h"
#include <SPI.h>
#include <TaskSchedulerDeclarations.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <PCF8574.h>
#include <map>              // user must include to use std::map (see above comment)

#include "shared/SettingBase/SettingBase.h"
#include "../../utils.h"
#include "../../CanbusMessage/CanbusMessage.h"
#include "../../SharedDefinitions.h"
#include "../../Logger/Logger.h"
#include "shared/enums/Event.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeWriteSetting/CarduinoNodeWriteSetting.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeCanGetHellos/CarduinoNodeCanGetHellos.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeCanPowerEvents/CarduinoNodeCanPowerEvents.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeCanGetSettings/CarduinoNodeCanGetSettings.h"
#include "shared/executors/Executors.h"

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

#define DIGITAL_PINS_UPDATE_INTERVAL 20
#define WRITE_SETTINGS_ON_EEPROM_INTERVAL 30000

struct PinInformation {
    uint8_t pin;
    PCF8574 *pcf8574;
    bool isHigh;
    bool hasChanged;
    std::function<void(PinInformation*)> onChange;
};

// class Executors; // forward declaration to avoid circular dependency
class CarduinoNode : public Logger, public SettingBase {
    private:
        String fallbackPageProcessor(const String& var);
        bool existsAllFiles();
        int requestsCounter;
        void setupServerWebapp();
        void setupServerFallback();
        bool _fallbackPage;

    public:
        uint8_t id;
        MCP_CAN *can;
        AsyncWebServer *server;
        String ssid;
        String password;
        int interruptPin;
        Executors *canExecutors;
        bool initializedCan;
        Scheduler *scheduler;
        Task *temperatureTask;
        std::map<uint8_t, PinInformation*> *pinInformations;

        CarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password, bool logOnServer, bool logOnSerial);
        
        /**
         * TODO: manage saving of settings in eeprom
        */

        void loop();
        void manageReceivedCanbusMessage(CanbusMessage *message);
        void sendByteCanbus(uint16_t messageId, int len, uint8_t buf[]);
        void sendCanbusMessage(CanbusMessage *message);
        bool availableCanbusMessages();
        void otaStartup();
        void otaShutdown();
        void restart();
        void sendHeartbeat();
        void sendEvent(const Event *event);
        void sendEvent(const Event *event, int receiverId);
        void addPinToRead(uint8_t pin, PCF8574 *pcf8574 = nullptr, std::function<void(PinInformation*)> onChange = nullptr);
        PinInformation* getPinInformation(uint8_t pin);
        void readDigitalPins();
        virtual void enable();
        virtual void disable();
        virtual void enableInterrupt();
        virtual void disableInterrupt();
        void delayTask(int delay, std::function<void()> lambdaCallback);

        static uint16_t generateId(const Category category, const Enum messageEnum);
        static uint16_t generateId(const Category category, uint8_t messageId);
};
