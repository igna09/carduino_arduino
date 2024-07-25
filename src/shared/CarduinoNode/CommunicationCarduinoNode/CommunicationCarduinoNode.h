#pragma once

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/enums/MediaControl.h"
#include "shared/CanbusMessage/MediaControlMessage/MediaControlMessage.h"
#include "shared/SharedDefinitions.h"
#include "utils/MyBLEServerCallbacks/MyBLEServerCallbacks.h"
#include "utils/GAPCallback/GAPCallback.h"
#include "shared/CarduinoNode/CommunicationCarduinoNode/executors/CommunicationCarduinoNodeEvents/CommunicationCarduinoNodeEvents.h"

#include <NimBLEDevice.h>

#define SLEEP_TIME 4
#define ON_TIME 1


class CommunicationCarduinoNode : public CarduinoNode {
    public:
        CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();
        void clientAuthenticated(NimBLEConnInfo);
        void clearWhitelist();
        int customGapCallback(ble_gap_event *event, void *arg);
        bool connected;
        Task* rssiTask;
        void enableNewPairing();
        void disableNewPairing();
        bool disabledPairing;
        void sendBLEPairingCode(int code);
        void onIdentity(NimBLEConnInfo info);

        void test() override;

    private:
        NimBLEServer* bleServer;
        NimBLEDevice* bleDevice;

        void listWhitelist();
        void backupWhitelist();
        void restoreWhitelist();
};
