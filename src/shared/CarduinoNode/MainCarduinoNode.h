#pragma once

#include <TaskSchedulerDeclarations.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "../callbacks/TemperatureCallback.h"
#include "../callbacks/LuminanceCallback.h"
#include "./CarduinoNode.h"

class MainCarduinoNode : public CarduinoNode {
    public:
        Scheduler* scheduler;
        Adafruit_AHTX0* aht;
        Scheduler *runner;
        Task *temperatureTask;
        Task *luminanceTask;

        MainCarduinoNode(int cs, int interruptPin, String ssid, String password);
        ~MainCarduinoNode();

        void luminanceCallback();
        void temperatureCallback();
        void manageReceivedMessage(CanbusMessage message);
        void loop();
};
