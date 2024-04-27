#pragma once

#include <TaskSchedulerDeclarations.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "shared/enums/Setting.h"
#include "shared/enums/CanbusMessageType.h"
#include "shared/enums/TypedEnum.h"
#include "callbacks/TemperatureCallback.h"
#include "callbacks/LuminanceCallback.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "executors/CarstatusExecutor.h"
#include "executors/AllMessageExecutor.h"
#include "executors/WriteSettingExecutor.h"
#include "executors/MediaControlExecutor.h"
#include "shared/executors/Executors.h"
#include "./executors/ReadSettingExecutor.h"
#include "./executors/HeartbeatExecutor.h"
#include <map>              // user must include to use std::map (see above comment)
#include <PCF8574.h>
#include "shared/SharedDefinitions.h"

struct SplittedUsbMessage {
    bool isValid;
    String messages[3];
};

class MainCarduinoNode : public CarduinoNode {
    public:
        Executors *usbExecutors;
        std::map<uint8_t, unsigned long> *lastReceivedHeartbeats;

        /**
         * TODO: move to new node to relief main node
        */
        Adafruit_AHTX0 *aht;
        Task *temperatureTask;
        Task *luminanceTask;
        PCF8574 *pcf8574;
        bool pressing;
        bool pairing;
        uint8_t pressedPin;
        unsigned long lastPressedMillis;

        MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid,  char *password);

        // void manageReceivedCanbusMessage(CanbusMessage message);
        void manageReceivedUsbMessage(CanbusMessage message);
        void loop();
        void sendSerialMessage(CanbusMessage *message);
        SplittedUsbMessage* splitReceivedUsbMessage(String message);
        void handleReceivedSerialMessage(String message);

        /**
         * TODO: move to new node to relief main node
        */
        void executeSwcCommand(MediaControl *mediaControl);
        void executeSwcPairing();
        void luminanceCallback();
        void temperatureCallback();
};
