#include "CarduinoNode.h"

CarduinoNode::CarduinoNode(int cs, int interruptPin, char *ssid, char *password) {
    //Serial.println("start CarduinoNode");
    this->can = new MCP_CAN(cs);
    this->server = new ESP8266WebServer(80);
    this->ssid = ssid;
    this->password = password;
    this->interruptPin = interruptPin;

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
    if(can->begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
        Serial.println("MCP2515 Initialized Successfully!");
    else
        Serial.println("Error Initializing MCP2515...");
    can->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
    pinMode(interruptPin, INPUT);                            // Configuring pin for /INT input
    this->otaMode = true;
    WiFi.mode(WIFI_OFF);
    //Serial.println("end CarduinoNode");
};

void CarduinoNode::loop() {
    if(otaMode) {
        if(WiFi.getMode() != WIFI_AP) {
            this->otaStartup();
            delay(2000);
        } else {
            /* HANDLE UPDATE REQUESTS */
            this->server->handleClient();
        }
    } else {
        if(WiFi.getMode() != WIFI_OFF) {
            this->otaShutdown();
        }
    }

    if(!digitalRead(this->interruptPin)) {                         // If CAN0_INT pin is low, read receive buffer
        while(CAN_MSGAVAIL == can->checkReceive()) {
            unsigned long id;
            uint8_t len;
            unsigned char buf[8];

            can->readMsgBuf(&id, &len, buf);
            //TODO: replace with factory
            CanbusMessage message(id, buf, len);
            
            manageReceivedMessage(message);
        }
    }
};

void CarduinoNode::manageReceivedMessage(CanbusMessage message) {};

void CarduinoNode::sendMessageCanBus(unsigned long messageId, int len, byte *buf) {
    byte sndStat = can->sendMsgBuf(0x100, 0, len, buf);
    if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
    } else {
        Serial.println("Error Sending Message...");
    }
};

void CarduinoNode::sendFloatMessageCanbus(unsigned long id, float v) {
    uint8_t buf[5];
    convertFloatToByteArray(buf, v);
    sendMessageCanBus(0, 5, buf);
};

void CarduinoNode::sendIntMessageCanbus(unsigned long id, uint32_t v) {
    uint8_t buf[4];
    convertIntegerToByteArray(buf, v);
    sendMessageCanBus(0, 4, buf);
};

void CarduinoNode::otaStartup() {
    WiFi.softAP(this->ssid, this->password);
    delay(250);
    IPAddress IP = IPAddress (10, 10, 10, 1);
    IPAddress NMask = IPAddress (255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);

    /* INITIALIZE ESP2SOTA LIBRARY */
    ESP2SOTA.begin(server);
    this->server->begin();
};

void CarduinoNode::otaShutdown() {
    this->server->stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
};
