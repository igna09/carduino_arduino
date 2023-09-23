#include "CarduinoNode.h"
#include "../../executors/Executors.h"

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
    this->executors = new Executors();
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

    if (availableCanbusMessages()) {
      // iterate over all pending messages
      // If either the bus is saturated or the MCU is busy,
      // both RX buffers may be in use and reading a single
      // message does not clear the IRQ conditon.
      while (CAN_MSGAVAIL == can->checkReceive()) {
        uint8_t len = 0;
        uint8_t buf[8];
        long unsigned int id;

        can->readMsgBuf(&id, &len, buf);
        CanbusMessage *m = new CanbusMessage(id, buf, len);

        manageReceivedCanbusMessage(m);

        delete m;
      }
    }
};

void CarduinoNode::manageReceivedCanbusMessage(CanbusMessage *message) {
    this->executors->execute(this, message);
};

void CarduinoNode::sendByteCanbus(uint16_t messageId, int len, uint8_t buf[]) {
    byte sndStat = can->sendMsgBuf(messageId, 0, len, buf);
    if(sndStat == CAN_OK){
        Serial.println("Message Sent Successfully!");
    } else {
        Serial.println("Error Sending Message...");
    }
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

uint16_t CarduinoNode::generateId(const Category category, const Enum messageEnum) {
    uint16_t id = category.id;
    id = (id << 8) | messageEnum.id;
    return id;
}

bool CarduinoNode::availableCanbusMessages() {
    return digitalRead(this->interruptPin) == LOW;
}

void CarduinoNode::sendCanbusMessage(CanbusMessage message) {
    sendByteCanbus(message.id, message.payloadLength, message.payload);
}
