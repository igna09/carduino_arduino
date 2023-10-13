#include "CarduinoNode.h"
#include "../../executors/Executors.h"

CarduinoNode::CarduinoNode(int cs, int interruptPin, const char *ssid, const char *password) {
    this->can = new MCP_CAN(cs);
    this->server = new ESP8266WebServer(80);
    this->ssid = ssid;
    this->password = password;
    this->interruptPin = interruptPin;

    // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
    if(can->begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK) {
        Serial.println("MCP2515 Initialized Successfully!");
        this->initializedCan = true;
    } else {
        Serial.println("Error Initializing MCP2515...");
        this->initializedCan = false;
    }
    can->setMode(MCP_NORMAL);                     // Set operation mode to normal so the MCP2515 sends acks to received data.
    pinMode(interruptPin, INPUT);                            // Configuring pin for /INT input

    this->otaMode = false;
    WiFi.mode(WIFI_OFF);
    
    this->canExecutors = new Executors();
};

void CarduinoNode::loop() {
    if(otaMode) {
        this->server->handleClient();
    }

    if (initializedCan && availableCanbusMessages()) {
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
    delay(250);
    IPAddress IP = IPAddress (10, 10, 10, 1);
    IPAddress NMask = IPAddress (255, 255, 255, 0);
    WiFi.softAPConfig(IP, IP, NMask);
    IPAddress myIP = WiFi.softAPIP();
    //Serial.print("AP IP address: ");
    //Serial.println(myIP);

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
    uint8_t v = digitalRead(this->interruptPin);
    // Serial.print("CarduinoNode::availableCanbusMessages ");
    // Serial.println(v);
    return v == LOW;
}

void CarduinoNode::sendCanbusMessage(CanbusMessage *message) {
	// printUint8Array("loop", m.payload, m.payloadLength);
    sendByteCanbus(message->id, message->payloadLength, message->payload);
}
