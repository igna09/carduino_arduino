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
#include "shared/utils.h"
#include "shared/CanbusMessage/CanbusMessage.h"
#include "shared/SharedDefinitions.h"
#include "shared/Logger/Logger.h"
#include "shared/enums/Event.h"
#include "shared/CanbusMessage/EventMessage/EventMessage.h"
#include "shared/CanbusMessage/LogMessage/LogMessage.h"
#include "executors/CarduinoNodeWriteSetting/CarduinoNodeWriteSetting.h"
#include "CarduinoNode/CarduinoNode/executors/CarduinoNodeCanEvent/CarduinoNodeCanEvent.h"
#include "CarduinoNode/CarduinoNode/executors/CarduinoNodeCanGetSettings/CarduinoNodeCanGetSettings.h"
#include "CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialGetSettings/CarduinoNodeSerialGetSettings.h"
#include "CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialWriteSetting/CarduinoNodeSerialWriteSetting.h"
#include "CarduinoNode/CarduinoNode/executors/CarduinoNodeSerialEvent/CarduinoNodeSerialEvent.h"
// #include "shared/executors/Executor.h"

/**
 * This node has got a canbus interface and a wifi AP to update software
*/

const char FALLBACK_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html lang="en"><head><meta name="viewport" content="width=device-width,initial-scale=1"><meta charset="UTF-8"><script type="text/javascript">let baseUrl,currentPath=(baseUrl=window.location.origin.includes("file://")?"http://localhost":window.location.protocol+"//"+window.location.host,document.write("<base href='"+baseUrl+"' />"),"/");function post(e,t,n){httpRequest("POST",e,t,n)}function get(e,t){httpRequest("GET",e,void 0,t)}function httpRequest(e,t,n,l){let i=new XMLHttpRequest;i.open(e,t),i.setRequestHeader("Content-Type","application/json"),i.setRequestHeader("Accept","application/json"),i.setRequestHeader("Access-Control-Allow-Origin","*"),i.onload=e=>{if(200<=i.status&&i.status<300){if(l){let e=i.response&&0<i.response.length?JSON.parse(i.response):{};l(e)}}else console.error(i.statusText,i.response)},n?i.send(JSON.stringify(n)):i.send()}function humanFileSize(e,t=!1,n=1){var l=t?1e3:1024;if(Math.abs(e)<l)return e+" B";var i=t?["kB","MB","GB","TB","PB","EB","ZB","YB"]:["KiB","MiB","GiB","TiB","PiB","EiB","ZiB","YiB"];let o=-1;for(var r=10**n;e/=l,++o,Math.round(Math.abs(e)*r)/r>=l&&o<i.length-1;);return e.toFixed(n)+" "+i[o]}function getAllFiles(e){post("/file-list",{path:currentPath=e},e=>{var t=document.getElementById("file-list");t.innerHTML="";let i=document.createElement("table");var n=i.insertRow(),l=n.insertCell(),o=n.insertCell(),n=n.insertCell();l.textContent="Name",n.textContent="Action",o.textContent="Size",e.forEach(e=>{var t=i.insertRow(),n=t.insertCell(),l=t.insertCell(),t=t.insertCell(),n=(n.textContent=e.name,l.textContent=humanFileSize(e.size),document.createElement("button")),l=(n.textContent="Open",n.addEventListener("click",()=>{"file"===e.type?window.open(""+currentPath+e.name):"folder"===e.type&&getAllFiles(currentPath+e.name+"/")}),document.createElement("button"));l.textContent="Delete",l.addEventListener("click",()=>{deleteFile(currentPath+e.name)}),t.appendChild(n),t.appendChild(l)}),t.appendChild(i),document.getElementById("home-button").disabled="/"===currentPath,document.getElementById("path-label").textContent=currentPath})}function goHome(){getAllFiles("/")}function deleteFile(e){post("/delete-file",{filename:e},e=>{getAllFiles(currentPath)})}function refreshFiles(){getAllFiles(currentPath)}getAllFiles("/")</script></head><body><p></p><h1>Files</h1><button onclick="refreshFiles()">Aggiorna</button> <button id="home-button" onclick="goHome()" disabled="disabled">Torna alla home</button> <span>Path:</span> <span id="path-label"></span><div id="file-list"></div><p></p><p></p><h1>File upload</h1><form method="POST" action="/file-upload" enctype="multipart/form-data"><span><input type="file" name="file" multiple="multiple"></span><input type="submit" name="upload" value="Upload" title="Upload File"></form><span>If you are seeing this page means that not every angular has been uploaded (favicon.ico.gz index.html.gz main.js.gz polyfills.js.gz styles.css.gz)</span><p></p></body></html>
)rawliteral";

#define DIGITAL_PINS_UPDATE_INTERVAL 20
#define WRITE_SETTINGS_ON_EEPROM_INTERVAL 30000
#define CAN_MESSAGE_VALUES_BUFFER_CHUNK_SIZE 5
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
        void setupServerAPI();
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
        void addPinToRead(uint8_t pin, std::function<void(PinInformation*)> onChange = nullptr);
        PinInformation* getPinInformation(uint8_t pin);
        void readDigitalPins();
        virtual void enable();
        virtual void disable();
        virtual void enableInterrupt();
        virtual void disableInterrupt();
        Task* delayTask(int delay, std::function<void()> lambdaCallback);
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
        void onOnlineOfflineEvent(OnlineEnum event);

        static uint16_t generateId(const Category category, const Enum messageEnum);
        static uint16_t generateId(const Category category, uint8_t messageId);

        virtual void test();
};
