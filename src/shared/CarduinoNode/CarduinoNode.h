#pragma once

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP2SOTA.h>

#include <mcp_can.h>
#include <SPI.h>

#include "../utils.h"
#include "../CanbusMessage/CanbusMessage.h"

class CarduinoNode {
    private:
        void otaStartup();
        void otaShutdown();

    public:
        MCP_CAN *can;
        ESP8266WebServer *server;
        String ssid;
        String password;
        bool otaMode;
        int interruptPin;

        CarduinoNode(int cs, int interruptPin, String ssid, String password);
        ~CarduinoNode();
        void loop();
        void manageReceivedMessage(CanbusMessage message);
        void sendMessageCanBus(unsigned long messageId, int len, byte *buf);
        void sendFloatMessageCanbus(unsigned long id, float v);
        void sendIntMessageCanbus(unsigned long id, uint32_t v);
};
