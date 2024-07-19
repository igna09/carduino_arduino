#pragma once

// #define EXTERNAL_SD

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable

#include <Arduino.h>
#include "mcp_can.h"
#include <SPI.h>
#include <TaskSchedulerDeclarations.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <PCF8574.h>
#include <map>              // user must include to use std::map (see above comment)
#include <FunctionalInterrupt.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <Update.h>
#endif

#include "shared/SettingBase/SettingBase.h"
#include "shared/FSBase/FSBase.h"
#include "../../utils.h"
#include "../../CanbusMessage/CanbusMessage.h"
#include "../../SharedDefinitions.h"
#include "../../Logger/Logger.h"
#include "shared/enums/Event.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/CanbusMessage/LogMessage/LogMessage.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeWriteSetting/CarduinoNodeWriteSetting.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeCanEvent/CarduinoNodeCanEvent.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeCanGetSettings/CarduinoNodeCanGetSettings.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialGetSettings/CarduinoNodeSerialGetSettings.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialWriteSetting/CarduinoNodeSerialWriteSetting.h"
#include "shared/CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialEvent/CarduinoNodeSerialEvent.h"
// #include "shared/executors/Executor.h"

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
#define CAN_MESSAGE_VALUES_BUFFER_CHUNK_SIZE 1
#define CAN_MESSAGE_VALUES_BUFFER_SIZE 32

struct SplittedUsbMessage {
    bool isValid;
    String messages[3];
};

struct PinInformation {
    uint8_t pin;
    PCF8574 *pcf8574;
    bool isHigh;
    bool hasChanged;
    std::function<void(PinInformation*)> onChange;
};

struct CanMessageValues {
    unsigned long id;
    uint8_t len = 0;
    uint8_t buf[8];
};

class Executor; // forward declaration to avoid circular dependency
class CarduinoNode : public Logger, public FSBase, public SettingBase {
    private:
        String fallbackPageProcessor(const String& var);
        bool existsAllFiles();
        int requestsCounter;
        void setupServerWebapp();
        void setupServerFallback();
        bool _fallbackPage;
        CanMessageValues* messageBuffer[CAN_MESSAGE_VALUES_BUFFER_SIZE];
        uint8_t nextMessageBufferIndexToInsert = 0;
        uint8_t nextMessageBufferIndexToRead = 0;
        void setupCanbus();
        void heartbeatWDT();

    public:
        uint8_t id;
        MCP_CAN *can;
        AsyncWebServer *server;
        String ssid;
        String password;
        int interruptPin;
        Executor *canExecutor;
        bool initializedCan;
        Scheduler *scheduler;
        Task *temperatureTask;
        Task* heartbeatWdtTask;
        std::map<uint8_t, PinInformation*> *pinInformations;
        bool isEnabled;
        Executor *usbExecutor;
        unsigned long lastTimeReceivedHeartbeat;

        CarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password, bool enableI2c = false, bool logOnServer = false, bool logOnSerial = false);

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
        virtual void sendLog(uint8_t id, int value);
        virtual void sendLog(uint8_t id, bool value);
        virtual void sendLog(uint8_t id, float value);
        void handleRxBuffer();
        void addCanMessageValuesToBuffer(CanMessageValues *canMessageValues);
        uint8_t getBufferSize();
        void readCanMessageFromMcpBuffer();
        void sendSerialMessage(CanbusMessage *message);
        SplittedUsbMessage* splitReceivedUsbMessage(String message);
        void handleReceivedSerialMessage(String message);
        void resetWebapp();

        static uint16_t generateId(const Category category, const Enum messageEnum);
        static uint16_t generateId(const Category category, uint8_t messageId);

        virtual void test();
};
