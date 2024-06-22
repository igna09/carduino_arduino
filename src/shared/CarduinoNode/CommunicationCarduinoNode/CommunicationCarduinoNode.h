#pragma once

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include "utils/MyBLESecurityCallbacks/MyBLESecurityCallbacks.h"
#include "utils/MyBLEServerCallbacks/MyBLEServerCallbacks.h"
#include "utils/GAPCallback/GAPCallback.h"

#include <BLEDevice.h>            // sets up BLE device constructs

// class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
// {
//     void onResult(BLEAdvertisedDevice advertisedDevice) 
//     {
//       String strName;
//       strName = advertisedDevice.getName().c_str();
//       if ( strName.length() > 0 )
//       {
//         // Serial.printf("Name: %s n\n", strName);
//         Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
//       }
//     }
// };

class CommunicationCarduinoNode : public CarduinoNode {
    public:
        CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();
        void clientAuthenticated();
        void clearWhitelist();
        void customGapCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
        void logToFile(String);
        bool authenticated;
        bool connected;
        BLEAddress* authenticatedBdAddress;

    private:
        BLEServer* bleServer;
        BLEDevice* bleDevice;
};
