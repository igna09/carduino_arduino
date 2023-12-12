#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TaskSchedulerDeclarations.h>
#include "../CarduinoNode/CarduinoNode.h"
#include "KLineKWP1281Lib.h"
#include "KlineEcuEnum.h"
#include "ValueToReadEnum.h"
#include "../../enums/CanbusMessageType.h"
#include "../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "KlineCallback.h"
#include "AfterReadExecutor/AfterReadExecutors.h"
#include "AfterReadExecutor/FuelConsumptionExecutor.h"

class KlineCarduinoNode : public CarduinoNode {
    public:
        KlineCarduinoNode(uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();

    private:
        KLineKWP1281Lib *kLine;
		SoftwareSerial *softwareSerial;
		uint8_t pin_rx;
    	uint8_t pin_tx;
        Scheduler *scheduler;
        Task *readValuesTask;
        KlineEcuEnum *lastConnectedEcu;
        AfterReadExecutors *afterReadExecutors;

        void readValues();
};
