#pragma once

#include <TaskSchedulerDeclarations.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "shared/enums/Setting.h"
#include "shared/enums/CanbusMessageType.h"
#include "shared/enums/TypedEnum.h"
#include "callbacks/TemperatureCallback.h"
#include "callbacks/LuminanceCallback.h"
#include "callbacks/TurnOffRadioCallback.h"
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "executors/CarstatusExecutor.h"
#include "executors/AllMessageExecutor.h"
#include "executors/WriteSettingExecutor.h"
#include "executors/MediaControlExecutor.h"
#include "shared/executors/Executors.h"
#include "./executors/HeartbeatExecutor.h"
#include <map>              // user must include to use std::map (see above comment)
#include <PCF8574.h>
#include "shared/SharedDefinitions.h"
#include "shared/enums/Event.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeCanReadSettingExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeSerialGetSettings.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeCanEvent/MainNodeCanEvent.h"

struct SplittedUsbMessage {
    bool isValid;
    String messages[3];
};

struct NodeInformation {
    uint8_t id;
    unsigned long lastTimeReceivedHeartBeat;
    Event *lastCompletedEvent;
};

class MainCarduinoNode : public CarduinoNode {
    public:
        Executors *usbExecutors;
        std::map<uint8_t, NodeInformation*> *nodeInformations;
        bool isRadioOn;
        bool isKeyOn;
        Task *turnOffRadioTask;

        MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid,  char *password);

        // void manageReceivedCanbusMessage(CanbusMessage message);
        void manageReceivedUsbMessage(CanbusMessage message);
        void loop();
        void sendSerialMessage(CanbusMessage *message);
        SplittedUsbMessage* splitReceivedUsbMessage(String message);
        void handleReceivedSerialMessage(String message);
        void startTurnOffSystem();
        void manageRadioPower();
        void pcfSetup();
        NodeInformation* getNodeInformation(uint8_t id);
        NodeInformation* createOrGetNodeInformation(uint8_t id);

        /**
         * TODO: move to new node to relief main node
        */
        Adafruit_AHTX0 *aht;
        Task *temperatureTask;
        Task *luminanceTask;
        PCF8574 *pcf8574;
        bool isPressing;
        bool isPairing;
        bool isWaitingPairing;
        uint8_t pressedPin;
        unsigned long lastPressedMillis;

        void executeSwcCommand(MediaControl *mediaControl);
        void manageSwc();
        void startSwcPairing();
        void luminanceCallback();
        void temperatureCallback();
};
