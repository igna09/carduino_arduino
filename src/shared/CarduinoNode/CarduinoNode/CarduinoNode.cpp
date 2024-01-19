#include "CarduinoNode.h"
#include "../../executors/Executors.h" // include here to avoid circular dependency
#include "WriteSetting.h" // include here to avoid circular dependency

CarduinoNode::CarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password, bool logOnServer, bool logOnSerial) : Logger() {
    this->id = id;
    this->can = new MCP_CAN(cs);
    this->server = new AsyncWebServer(80);
    this->ssid = ssid;
    this->password = password;
    this->interruptPin = interruptPin;

    if (!LittleFS.begin())
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        return;
    }

    //TODO: add management of simple page to upload webapp files for first time
    this->server->serveStatic("/", LittleFS, "/").setDefaultFile("/index.html");

    this->server->on("/update-firmware", HTTP_POST, [](AsyncWebServerRequest *request){
        AsyncWebServerResponse *response;
        if(Update.hasError()) {
            response = request->beginResponse(500, "text/plain", Update.getErrorString());
        } else {
            response = request->beginResponse(200, "text/plain");
        }
        response->addHeader("Connection", "close");
        request->send(response);
        ESP.restart();
    },[](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
        if(!index){
            Serial.printf("Update Start: %s\n", filename.c_str());
            Update.runAsync(true);
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
            request->_tempFile = LittleFS.open("/" + filename, "w");
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
            request->redirect("/");
        }
    });

    this->setupLogger(this->server, logOnServer, logOnSerial);

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

    this->otaMode = false;
    WiFi.mode(WIFI_OFF);
    
    this->canExecutors = new Executors();
    this->canExecutors->addExecutor(new WriteSetting());

    this->scheduler = new Scheduler();
    SendHeartbeatCallback<void(void)>::func = std::bind(&CarduinoNode::sendHeartbeat, this);
    new Task(HEARTBEAT_INTERVAL, TASK_FOREVER, static_cast<TaskCallback>(SendHeartbeatCallback<void(void)>::callback), this->scheduler, true);
    this->scheduler->startNow();
};

void CarduinoNode::loop() {
    this->scheduler->execute();

    // if(otaMode) {
    //     this->server->loop();
    // }

    if (initializedCan && availableCanbusMessages()) {
    //   Serial.println(initializedCan && availableCanbusMessages() ? "true" : "false");
      // iterate over all pending messages
      // If either the bus is saturated or the MCU is busy,
      // both RX buffers may be in use and reading a single
      // message does not clear the IRQ conditon.
      while (CAN_MSGAVAIL == can->checkReceive()) {
        uint8_t len = 0;
        uint8_t buf[8];
        long unsigned int id;

        can->readMsgBuf(&id, &len, buf);

        if(len > 0) {
            // printUint8Array("CarduinoNode::loop", buf, len);

            CanbusMessage *m = new CanbusMessage(id, buf, len);
            manageReceivedCanbusMessage(m);
            delete m;
        }
      }
    }
};

void CarduinoNode::manageReceivedCanbusMessage(CanbusMessage *message) {
    this->canExecutors->execute(this, message);
};

void CarduinoNode::sendByteCanbus(uint16_t messageId, int len, uint8_t *buf) {
    byte sndStat = can->sendMsgBuf(messageId, 0, len, buf);
    /* if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
    } else {
        Serial.println("Error Sending Message...");
    }*/
};

void CarduinoNode::otaStartup() {
    WiFi.softAP(this->ssid, this->password);
    delay(150);
    IPAddress IP = IPAddress (10, 10, 10, 10);
    IPAddress NMask = IPAddress (255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);

    this->server->begin();

    this->otaMode = true;
};

void CarduinoNode::otaShutdown() {
    this->server->end();

    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);

    this->otaMode = false;
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
    // Serial.println(message->id, 2);
	// printUint8Array("loop", message->payload, message->payloadLength);
    sendByteCanbus(message->id, message->payloadLength, message->payload);
}

void CarduinoNode::restart() {
    ESP.restart();
}

void CarduinoNode::sendHeartbeat() {
    HeartbeatMessage *heartbeatMessage = new HeartbeatMessage(this->id);
    sendCanbusMessage(heartbeatMessage);
    delete heartbeatMessage;
}
