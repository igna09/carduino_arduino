#pragma once

#include <TaskSchedulerDeclarations.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "../../callbacks/TemperatureCallback.h"
#include "../../callbacks/LuminanceCallback.h"
#include "../CarduinoNode/CarduinoNode.h"
#include "executors/CarstatusExecutor.h"

class MainCarduinoNode : public CarduinoNode {
    public:
        Scheduler *scheduler;
        Adafruit_AHTX0 *aht;
        Scheduler *runner;
        Task *temperatureTask;
        Task *luminanceTask;

        MainCarduinoNode(int cs, int interruptPin, char *ssid,  char *password);

        void luminanceCallback();
        void temperatureCallback();
        void manageReceivedCanbusMessage(CanbusMessage message);
        void manageReceivedUsbMessage(CanbusMessage message);
        void loop();
};
