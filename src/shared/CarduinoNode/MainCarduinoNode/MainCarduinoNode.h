#pragma once

#include <TaskSchedulerDeclarations.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "shared/enums/Setting.h"
#include "shared/enums/CanbusMessageType.h"
#include "shared/enums/TypedEnum.h"
#include "callbacks/TemperatureCallback.h"
#include "callbacks/LuminanceCallback.h"
#include "shared//CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "executors/CarstatusExecutor.h"
#include "executors/AllMessageExecutor.h"
#include "executors/WriteSetting.h"
#include "executors/MediaControlExecutor.h"
#include "shared/executors/Executors.h"
#include "./executors/ReadSettingExecutor.h"

struct SplittedUsbMessage {
    bool isValid;
    String messages[3];
};

class MainCarduinoNode : public CarduinoNode {
    public:
        Scheduler *scheduler;
        Adafruit_AHTX0 *aht;
        Scheduler *runner;
        Task *temperatureTask;
        Task *luminanceTask;
        Executors *usbExecutors;

        MainCarduinoNode(int cs, int interruptPin, char *ssid,  char *password);

        void luminanceCallback();
        void temperatureCallback();
        // void manageReceivedCanbusMessage(CanbusMessage message);
        void manageReceivedUsbMessage(CanbusMessage message);
        void loop();
        void sendSerialMessage(CanbusMessage *message);
        SplittedUsbMessage* splitReceivedUsbMessage(String message);
        void handleReceivedSerialMessage(String message);
};
