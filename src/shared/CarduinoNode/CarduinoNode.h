#pragma once

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP2SOTA.h>

#include <mcp_can.h>
#include <SPI.h>

#include "../utils.h"
#include "../CanbusMessage/CanbusMessage.h"

/**
 * Send message to android --> Category;payload;
 * READ_SETTINGS;auto_close_rearview_mirrors-true;
 * CAR_STATUS;INTERNAL_LUMINANCE-2700;
 * Receive message from android --> Category;paylod;
 * READ_SETTINGS;get;
 * WRITE_SETTING;auto_close_rearview_mirrors-false;
*/

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

        CarduinoNode(int cs, int interruptPin, char *ssid, char *password);
        void loop();
        void manageReceivedCanbusMessage(CanbusMessage message);
        void sendMessageCanBus(uint16_t messageId, int len, uint8_t buf[]);
        void sendFloatMessageCanbus(uint16_t id, float v);
        void sendIntMessageCanbus(uint16_t id, int v);
        bool availableCanbusMessages();

        static uint16_t generateId(Category, Enum);
};
