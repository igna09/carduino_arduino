#include "CarduinoNode.h"

CarduinoNode::CarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password, bool enableI2c, bool logOnServer, bool logOnSerial) : Logger(), SettingBase() {
    this->id = id;
    this->can = new MCP_CAN(cs);
    this->server = new AsyncWebServer(80);
    this->server->end();
    this->ssid = ssid;
    this->password = password;
    this->interruptPin = interruptPin;
    this->_fallbackPage = true;
    this->_originalLogOnSerial = logOnSerial;
    this->_originalLogOnWebserver = logOnServer;
    this->isEnabled = false;

    #if defined(EXTERNAL_SD)
    fs = &SD;
    SD.begin(1);
    #else
        fs = &LittleFS;
        #ifdef ESP8266
        if(!LittleFS.begin()){
            Serial.println("LittleFS Mount Failed");
            return;
        }
        #elif defined(ESP32)
        if(!LittleFS.begin(true, "/")){
            Serial.println("LittleFS Mount Failed");
            return;
        }
        #endif
    #endif

    if(existsAllFiles()) {
        setupServerWebapp();
    } else {
        setupServerFallback();
    }

    if(enableI2c) {
        Wire.begin(NODE_SDA, NODE_SCL);
    }

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
    if(can->begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        this->printlnWrapper("MCP2515 Initialized Successfully!");
        this->initializedCan = true;
    } else {
        printlnWrapper("Error Initializing MCP2515...");
        this->initializedCan = false;
    }
    can->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
    pinMode(interruptPin, INPUT);                            // Configuring pin for /INT input
    attachInterrupt(digitalPinToInterrupt(interruptPin), std::bind(&CarduinoNode::readCanMessageFromMcpBuffer, this), FALLING);

    this->pinInformations = new std::map<uint8_t, PinInformation*>();

    this->addSetting(&Setting::RESTART, false, [&](SettingInformation *settingInformation){
        if(settingInformation->value->boolValue) {
            this->delayTask(1000, [&](){
                this->restart();
            });
        }
    });
    this->addSetting(&Setting::OTA_MODE, false, [&](SettingInformation *settingInformation){
        if(settingInformation->value->boolValue) {
            this->otaStartup();
        } else {
            this->otaShutdown();
        }
    }, true);
    WiFi.mode(WIFI_OFF);
    
    this->canExecutors = new Executor();
    this->canExecutors->addExecutor(new CarduinoNodeWriteSetting());
    this->canExecutors->addExecutor(new CarduinoNodeCanGetHellos());
    this->canExecutors->addExecutor(new CarduinoNodeCanPowerEvents());
    this->canExecutors->addExecutor(new CarduinoNodeCanGetSettings());

    this->scheduler = new Scheduler();
    this->scheduler->startNow();

    new Task(HEARTBEAT_INTERVAL, TASK_FOREVER, std::bind(&CarduinoNode::sendHeartbeat, this), this->scheduler, true);
    // SecondaryLoopCallback<void(void)>::func = std::bind(&CarduinoNode::secondaryLoopCallback, this);
    // new Task(100, TASK_FOREVER, static_cast<TaskCallback>(SecondaryLoopCallback<void(void)>::callback), this->scheduler, true);
    new Task(DIGITAL_PINS_UPDATE_INTERVAL, TASK_FOREVER, std::bind(&CarduinoNode::readDigitalPins, this), this->scheduler, true);

    this->sendEvent(&Event::HELLO);

    // if(!this->settingsLoaded) {
    //     this->restoreSettings();
    //     this->printlnWrapper("CarduinoNode::CarduinoNode settings loaded");
    // }
    Task *backupSettingsTask = new Task(WRITE_SETTINGS_ON_EEPROM_INTERVAL, TASK_FOREVER, [&](){
        this->backupSettings();
        this->printlnWrapper("CarduinoNode::CarduinoNode LAMBDA settings saved");
    }, this->scheduler);
    backupSettingsTask->restartDelayed();
    
    if(!this->initializedCan) {
        this->otaStartup();
    }
};

String CarduinoNode::fallbackPageProcessor(const String& var) {
    // if (var == "FILELIST") {
    //     return listFiles(true);
    // }
    // if (var == "FREESPIFFS") {
    //     return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
    // }

    // if (var == "USEDSPIFFS") {
    //     return humanReadableSize(SPIFFS.usedBytes());
    // }

    // if (var == "TOTALSPIFFS") {
    //     return humanReadableSize(SPIFFS.totalBytes());
    // }

  return String();
}

bool CarduinoNode::existsAllFiles() {
    bool mainJsExists = fs->exists("/main.js.gz");
    bool polyfillsJsExists = fs->exists("/polyfills.js.gz");
    bool indexHtmlExists = fs->exists("/index.html.gz");
    bool stylesCssExists = fs->exists("/styles.css.gz");

    return mainJsExists && polyfillsJsExists && indexHtmlExists && stylesCssExists;
}

void CarduinoNode::setupServerWebapp() {
    this->_fallbackPage = false;
    this->setupLogger(this->server, false, this->_originalLogOnSerial);
    this->server->serveStatic("/", *fs, "/").setDefaultFile("/index.html");

    this->server->on("/update-firmware", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response;
        if(Update.hasError()) {
            #if defined(ESP8266)
            response = request->beginResponse(500, "text/plain", Update.getErrorString());
            #elif defined(ESP32)
            response = request->beginResponse(500, "text/plain", Update.errorString());
            #endif
        } else {
            response = request->beginResponse(200, "text/plain");
        }
        response->addHeader("Connection", "close");
        request->send(response);
        ESP.restart();
    },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if(!index){
            Serial.printf("Update Start: %s\n", filename.c_str());
            #if defined(ESP8266)
            Update.runAsync(true);
            #endif
            if(!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)){
                Update.printError(Serial);
            }
        }
        if(!Update.hasError()){
            if(Update.write(data, len) != len){
                Update.printError(Serial);
            }
        }
        if(final){
            if(Update.end(true)){
                Serial.printf("Update Success: %uB\n", index+len);
            } else {
                Update.printError(Serial);
            }
        }
    });

    this->server->on("/file-upload", HTTP_POST, [&](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain");
        response->addHeader("Connection", "close");
        request->send(response);
    },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if (!index) {
            printlnWrapper("Upload Start: " + String(filename));
            // open the file on first call and store the file handle in the request object
            request->_tempFile = fs->open("/" + filename, "w");
        }

        if (len) {
            // stream the incoming chunk to the opened file
            request->_tempFile.write(data, len);
            // printlnWrapper("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
        }

        if (final) {
            // close the file handle as the upload is now done
            request->_tempFile.close();
            printlnWrapper("Upload Complete: " + String(filename) + ",size: " + String(index + len));
        }
    });

    this->server->on("/restart", HTTP_GET, [&](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(200, "text/plain");
        response->addHeader("Connection", "close");
        request->send(response);

        delay(250);
        this->restart();
    });

    this->server->on("/status", HTTP_GET, [&](AsyncWebServerRequest *request){
        AsyncResponseStream *response = request->beginResponseStream("application/json");
        JsonDocument jsonDocument;
        jsonDocument["freeHeap"] = ESP.getFreeHeap();
        jsonDocument["ssid"] = WiFi.softAPSSID();
        serializeJson(jsonDocument, *response);
        request->send(response);
    });

    this->server->on("/download", HTTP_GET, [&](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response = request->beginResponse(*fs, "/logs.txt", String(), true);
    });

    this->server->on(
        "/download",
        HTTP_POST,
        [&](AsyncWebServerRequest *request){
            if(request->hasParam("body", true)) {
                JsonDocument doc;
                if (deserializeJson(doc, request->getParam("body", true)->value())) {
                    request->send(500, "text/plain", "error deserializing");
                }

                String filename = doc["filename"];
                AsyncWebServerResponse *response = request->beginResponse(*fs, "/" + filename, String(), true);
            } else {
                request->send(500, "text/plain", "No body?!\n");
            }
        }//,
        // [&](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){},
        // [&](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        //     request->
        // }
    );
}

void CarduinoNode::setupServerFallback() {
    this->_fallbackPage = true;
    this->setupLogger(this->server, false, this->_originalLogOnSerial);

    this->server->on("/", HTTP_GET, [&](AsyncWebServerRequest * request) {
        String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
        printlnWrapper(logmessage);
        request->send_P(200, "text/html", FALLBACK_PAGE/*, [&](const String& var){
            return this->fallbackPageProcessor(var);
        }*/);
    });

    this->requestsCounter = 0;
    this->server->on("/file-upload", HTTP_POST, [&](AsyncWebServerRequest *request){
        if(this->existsAllFiles() && this->requestsCounter == 0) {
            this->server->reset();
            this->setupServerWebapp();
        }
        request->redirect("/");
    },[&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if (!index) {
            this->requestsCounter++;
            printlnWrapper("Upload Start: " + String(filename));
            // open the file on first call and store the file handle in the request object
            request->_tempFile = fs->open("/" + filename, "w");
        }

        if (len) {
            // stream the incoming chunk to the opened file
            request->_tempFile.write(data, len);
            // printlnWrapper("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
        }

        if (final) {
            this->requestsCounter--;
            // close the file handle as the upload is now done
            request->_tempFile.close();
            printlnWrapper("Upload Complete: " + String(filename) + ",size: " + String(index + len));
        }
    });
}

void CarduinoNode::loop() {
    this->scheduler->execute();

    handleRxBuffer();
};

void CarduinoNode::readCanMessageFromMcpBuffer() {
    while (CAN_MSGAVAIL == can->checkReceive()) {
        CanMessageValues *canMessageValues = new CanMessageValues();
        can->readMsgBuf(&canMessageValues->id, &canMessageValues->len, canMessageValues->buf);
        this->addCanMessageValuesToBuffer(canMessageValues);
    }
}

void CarduinoNode::addCanMessageValuesToBuffer(CanMessageValues *canMessageValues) {
    this->messageBuffer[this->nextMessageBufferIndexToInsert] = canMessageValues;
    this->nextMessageBufferIndexToInsert++;
    /**
     * TODO: manage possibility nextMessageBufferIndexToInsert > nextMessageBufferIndexToRead ==> buffer overflow ==> memory leak
     */
    if(this->nextMessageBufferIndexToInsert >= CAN_MESSAGE_VALUES_BUFFER_SIZE) {
        this->nextMessageBufferIndexToInsert = 0;
    }
}

uint8_t CarduinoNode::getBufferSize() {
    uint8_t value = this->nextMessageBufferIndexToInsert;
    if(this->nextMessageBufferIndexToInsert < this->nextMessageBufferIndexToRead) {
        value += CAN_MESSAGE_VALUES_BUFFER_SIZE;
    }
    value -= this->nextMessageBufferIndexToRead;
    return value;
}

void CarduinoNode::handleRxBuffer() {
    // Serial.println(getBufferSize());
    uint8_t i = 0;
    while(this->nextMessageBufferIndexToRead != this->nextMessageBufferIndexToInsert && i < CAN_MESSAGE_VALUES_BUFFER_CHUNK_SIZE) {
        CanMessageValues *canMessageValues = messageBuffer[this->nextMessageBufferIndexToRead];

        if(canMessageValues->len > 0) {
            // printUint8Array("CarduinoNode::loop", canMessageValues->buf, canMessageValues->len);

            CanbusMessage *m = new CanbusMessage(canMessageValues->id, canMessageValues->buf, canMessageValues->len);
            manageReceivedCanbusMessage(m);
            delete m;
        }

        delete canMessageValues;

        this->nextMessageBufferIndexToRead++;
        if(this->nextMessageBufferIndexToRead >= CAN_MESSAGE_VALUES_BUFFER_SIZE) {
            this->nextMessageBufferIndexToRead = 0;
        }
        i++;
    }
}

void CarduinoNode::manageReceivedCanbusMessage(CanbusMessage *message) {
    /**
     * moved to executor so that i can show this message only in messages i will read
     */
    // if(this->_logOnSerial || this->_logOnServer) {
    //     Category *category = (Category*)Category::getValueById(message->categoryId);
    //     if(category->createSpecializedCopyFunction != nullptr) {
    //         CanbusMessage *specialized = category->createSpecializedCopyFunction(message);
    //         this->printlnWrapper("CarduinoNode::manageReceivedCanbusMessage " + specialized->toSerialHumanString());
    //         delete specialized;
    //     } else {
    //         this->printlnWrapper("CarduinoNode::manageReceivedCanbusMessage " + message->toSerialHumanString());
    //     }
    // }
    this->canExecutors->execute(this, message);
};

void CarduinoNode::sendByteCanbus(uint16_t messageId, int len, uint8_t *buf) {
    byte sndStat = can->sendMsgBuf(messageId, 0, len, buf);
    // if(sndStat == CAN_OK){
    //     Serial.println("Message Sent Successfully!");
    // }
    // if(sndStat != CAN_OK){
    //     Serial.println("Error Sending Message... " + String(sndStat));
    // }
};

void CarduinoNode::otaStartup() {
    WiFi.softAP(this->ssid, this->password);
    delay(50);
    IPAddress IP = IPAddress (10, 10, 10, 10);
    IPAddress NMask = IPAddress (255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);

    this->server->begin();
    this->_logOnServer = _originalLogOnWebserver && !this->_fallbackPage;

    this->putSettingValue(&Setting::OTA_MODE, true);
};

void CarduinoNode::otaShutdown() {
    this->server->end();
    this->_logOnServer = false;

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    this->putSettingValue(&Setting::OTA_MODE, false);
};

uint16_t CarduinoNode::generateId(const Category category, const Enum messageEnum) {
    return generateId(category, messageEnum.id);
}

uint16_t CarduinoNode::generateId(const Category category, uint8_t messageId) {
    uint16_t id = category.id;
    id = (id << 8) | messageId;
    return id;
}

bool CarduinoNode::availableCanbusMessages() {
    uint8_t v = digitalRead(this->interruptPin);
    // Serial.print("CarduinoNode::availableCanbusMessages ");
    // Serial.println(v);
    return v == LOW;
}

void CarduinoNode::sendCanbusMessage(CanbusMessage *message) {
    if(initializedCan) {
        this->printlnWrapper("CarduinoNode::sendCanbusMessage " + message->toSerialHumanString());
        sendByteCanbus(message->id, message->payloadLength, message->payload);
    } else {
        this->printlnWrapper("CarduinoNode::sendCanbusMessage CAN not initialized, cannot send message " + message->toSerialHumanString());
    }
}

void CarduinoNode::restart() {
    this->printlnWrapper("CarduinoNode::restart");
    ESP.restart();
}

void CarduinoNode::sendHeartbeat() {
    EventMessage *eventMessage = new EventMessage(&Event::HEARTBEAT, this->id);
    sendCanbusMessage(eventMessage);
    delete eventMessage;
}

void CarduinoNode::sendEvent(const Event *event) {
    this->sendEvent(event, this->id);
}

void CarduinoNode::sendEvent(const Event *event, int nodeId) {
    EventMessage *eventMessage = new EventMessage(event, nodeId);
    this->sendCanbusMessage(eventMessage);
    delete eventMessage;
}

void CarduinoNode::enable() {
    this->printlnWrapper("CarduinoNode::enable");
	
	sendLog(0, true);

    this->isEnabled = true;
}

void CarduinoNode::disable() {
    this->printlnWrapper("CarduinoNode::disable");
	
	sendLog(20, true);

    this->isEnabled = false;
}

void CarduinoNode::enableInterrupt() {
    this->printlnWrapper("CarduinoNode::enableInterrupt");

    this->isEnabled = false;
}

void CarduinoNode::disableInterrupt() {
    this->printlnWrapper("CarduinoNode::disableInterrupt");

    this->isEnabled = true;
}

void CarduinoNode::addPinToRead(uint8_t pin, PCF8574 *pcf8574, std::function<void(PinInformation*)> onChange) {
    PinInformation *pinInformation = new PinInformation();
    pinInformation->pin = pin;
    pinInformation->pcf8574 = pcf8574;
    pinInformation->isHigh = false;
    pinInformation->hasChanged = false;
    pinInformation->onChange = onChange;

    (*this->pinInformations)[pin] = pinInformation;
}

PinInformation* CarduinoNode::getPinInformation(uint8_t pin) {
    std::map<uint8_t, PinInformation*>::iterator it = this->pinInformations->find(id);
    if (it != this->pinInformations->end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

void CarduinoNode::readDigitalPins() {
    std::map<uint8_t, PinInformation*>::iterator it;

    for (it = this->pinInformations->begin(); it != this->pinInformations->end(); it++) {
        PinInformation *pinInformation = it->second;
        bool isHigh;

        if(pinInformation->pcf8574 == nullptr) {
            isHigh = digitalRead(pinInformation->pin) == HIGH;
        } else {
            isHigh = pinInformation->pcf8574->digitalRead(pinInformation->pin, true) == HIGH;
        }
        // printlnWrapper(String(pinInformation->pin) + " " + String(isHigh));

        bool hasChanged = pinInformation->isHigh != isHigh;
        pinInformation->isHigh = isHigh;
        pinInformation->hasChanged = hasChanged;

        if(hasChanged && pinInformation->onChange != nullptr) {
            pinInformation->onChange(it->second);
        }
    }
}

void CarduinoNode::delayTask(int delay, std::function<void()> lambdaCallback) {
	this->printlnWrapper("CarduinoNode::delayTask " + String(millis()));
    Task *lambdaTask = new Task(delay, 1, lambdaCallback, this->scheduler);
    lambdaTask->setSelfDestruct(true);
    lambdaTask->restartDelayed();
}

void CarduinoNode::sendLog(uint8_t id, bool value) {
    LogMessage *logMessage = new LogMessage(this->id, id, value);
	this->sendCanbusMessage(logMessage);
    delete logMessage;
}

void CarduinoNode::sendLog(uint8_t id, float value) {
    LogMessage *logMessage = new LogMessage(this->id, id, value);
	this->sendCanbusMessage(logMessage);
    delete logMessage;
}

void CarduinoNode::sendLog(uint8_t id, int value) {
    LogMessage *logMessage = new LogMessage(this->id, id, value);
	this->sendCanbusMessage(logMessage);
    delete logMessage;
}
