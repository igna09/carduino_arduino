#pragma once

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>

#include "mcp_can.h"
#include <SPI.h>

#include "../../utils.h"
#include "../../CanbusMessage/CanbusMessage.h"

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

class WriteOtaSetting; // forward declaration to avoid circular dependency
class Executors; // forward declaration to avoid circular dependency
class CarduinoNode {
    private:

    public:
        MCP_CAN *can;
        ESP8266WebServer *server;
        String ssid;
        String password;
        bool otaMode;
        int interruptPin;
        Executors *canExecutors;
        bool initializedCan;
        ESP8266HTTPUpdateServer *httpUpdater;

        CarduinoNode(int cs, int interruptPin, const char *ssid, const char *password);

        void loop();
        void manageReceivedCanbusMessage(CanbusMessage *message);
        void sendByteCanbus(uint16_t messageId, int len, uint8_t buf[]);
        void sendCanbusMessage(CanbusMessage *message);
        bool availableCanbusMessages();
        void otaStartup();
        void otaShutdown();

        static uint16_t generateId(const Category category, const Enum messageEnum);
        static uint16_t generateId(const Category category, uint8_t messageId);
};
