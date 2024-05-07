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
#include "shared/executors/Executors.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/CarstatusExecutor/CarstatusExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/AllMessageExecutor/AllMessageExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/WriteSettingExecutor/WriteSettingExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MediaControlExecutor/MediaControlExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeCanReadSettingExecutor/MainNodeCanReadSettingExecutor.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeSerialGetSettings/MainNodeSerialGetSettings.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/MainNodeCanEvent/MainNodeCanEvent.h"
#include "shared/CarduinoNode/MainCarduinoNode/executors/HeartbeatExecutor/HeartbeatExecutor.h"
#include <map>              // user must include to use std::map (see above comment)
#include <PCF8574.h>
#include "shared/SharedDefinitions.h"
#include "shared/enums/Event.h"

#define RADIO_TURN_OFF_TIMER 15000

#define SWC_PRESS_INTERVAL  65
#define SWC_PAIRING_INTERVAL  5000
#define SWC_WAITING_PAIRING_INTERVAL  1000
#define SWC_FIRST_WAITING_PAIRING_INTERVAL  5000
#define SWC_PIN_SIZE 8

#define RADIO_MOSFET_PIN D4
#define ACCESSORY_12_V_PIN D5

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
        // bool canTurnOff;
        Task *turnOffRadioTask;

        MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid,  char *password);

        // void manageReceivedCanbusMessage(CanbusMessage message);
        void manageReceivedUsbMessage(CanbusMessage message);
        void loop();
        void sendSerialMessage(CanbusMessage *message);
        SplittedUsbMessage* splitReceivedUsbMessage(String message);
        void handleReceivedSerialMessage(String message);
        void turnOffSystem();
        void manageRadioPower();
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

        void pcfSetup();
        void executeSwcCommand(MediaControl *mediaControl);
        void manageSwc();
        void startSwcPairing();
        void luminanceCallback();
        void temperatureCallback();
        void voltageCallback();
};
