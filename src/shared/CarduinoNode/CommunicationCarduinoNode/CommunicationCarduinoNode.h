#pragma once

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"

// #include <BLEDevice.h>
// #include <BLEUtils.h>
// #include <BLEServer.h>
// #include <BLEClient.h>

#include <BLEDevice.h>            // sets up BLE device constructs
#include <BLEUtils.h>             // various BLE utilities for processing BLE data
#include <BLEScan.h>              // contains BLE scanning functions
#include <BLEAdvertisedDevice.h>  // contains BLE device characteristic data

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class MyCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        if (pCharacteristic->getValue().length() > 0) {
            Serial.println("Read request received!");
            // You can add your custom read handling code here
        }
    }
};

class MySecurity : public BLESecurityCallbacks {
    uint32_t onPassKeyRequest() {
        ESP_LOGI(LOG_TAG, "PassKeyRequest");
        Serial.println("PassKeyRequest");
        return 123456;
    }

    void onPassKeyNotify(uint32_t pass_key) {
        ESP_LOGI(LOG_TAG, "The passkey Notify number:%d", pass_key);
        Serial.print("The passkey Notify number: ");
        Serial.println(pass_key);
    }

    bool onConfirmPIN(uint32_t pass_key) {
        ESP_LOGI(LOG_TAG, "The passkey YES/NO number:%d", pass_key);
        Serial.print("The passkey YES/NO number: ");
        Serial.println(pass_key);
        vTaskDelay(5000);
        return true;
    }

    bool onSecurityRequest() {
        ESP_LOGI(LOG_TAG, "SecurityRequest");
        Serial.println("SecurityRequest");
        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) {
        Serial.println("Starting BLE work!");
        ESP_LOGI(LOG_TAG, "Starting BLE work!");
    }
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
    void onResult(BLEAdvertisedDevice advertisedDevice) 
    {
      String strName;
      strName = advertisedDevice.getName().c_str();
      if ( strName.length() > 0 )
      {
        // Serial.printf("Name: %s n\n", strName);
        Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      }
    }
};

class MyServerCallbacks: public BLEServerCallbacks 
{
    void onConnect(BLEServer* pServer) {
      Serial.println("onConnect");
    }
    void onDisconnect(BLEServer* pServer) {
      Serial.println("onDisconnect");
    }
};

class CommunicationCarduinoNode : public CarduinoNode {
    public:
        CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();

    private:
        // BLEServer* pServer;
        // BLECharacteristic* pCharacteristic;
        // uint8_t counter;
        // BLEClient* bleClient;
        
        int scanTime = 5; //In seconds
        BLEScan* pBLEScan;
};
