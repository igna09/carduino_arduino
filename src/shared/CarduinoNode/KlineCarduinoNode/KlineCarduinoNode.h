#pragma once

#define _TASK_STD_FUNCTION   // Compile with support for std::function 
#define _TASK_SELF_DESTRUCT      // Enable tasks to "self-destruct" after disable

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TaskSchedulerDeclarations.h>
#include "../CarduinoNode/CarduinoNode.h"
#include "KLineKWP1281Lib.h"
#include "KlineEcuEnum.h"
#include "ValueToReadEnum.h"
#include "../../enums/CanbusMessageType.h"
#include "../../CanbusMessage/CarstatusMessage/CarstatusMessage.h"
#include "AfterReadExecutor/AfterReadExecutors.h"
#include "AfterReadExecutor/FuelConsumptionExecutor.h"
// #include "KlineCallback.h"

class KlineCarduinoNode : public CarduinoNode {
    public:
        KlineCarduinoNode(uint8_t id, uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();

    private:
        KLineKWP1281Lib *kLine;
		EspSoftwareSerial::UART *softwareSerial;
		uint8_t pin_rx;
    	uint8_t pin_tx;
        Task *readValuesTask;
        KlineEcuEnum *lastConnectedEcu;
        AfterReadExecutors *afterReadExecutors;
        bool klineConnected;

        void readValues();
};
