#pragma once

#include <TaskScheduler.h>
#include <Adafruit_AHTX0.h>
#include <functional>
#include "../callbacks/Callback.h"
#include "./CarduinoNode.h"

class MainCarduinoNode : public CarduinoNode {
    public:
        Scheduler* scheduler;
        Adafruit_AHTX0* aht;

        MainCarduinoNode(int cs, int interruptPin, String ssid, String password, Scheduler* scheduler);
        ~MainCarduinoNode();

        void luminanceCallback();

        void temperatureCallback();

        void manageReceivedMessage(CanbusMessage message);
};
