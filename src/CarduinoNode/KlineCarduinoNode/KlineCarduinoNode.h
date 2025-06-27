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
#include "AfterReadExecutor/AfterReadExecutor.h"
#include "AfterReadExecutor/FuelConsumptionExecutor.h"
// #include "KlineCallback.h"

#define VOLTAGE_READING_INTERVAL 5000
#define R1 6800.0  // 6.8kΩ
#define R2 2000.0  // 2kΩ
#define VOLTAGE_READING_PIN A0

class KlineCarduinoNode : public CarduinoNode {
    public:
        KlineCarduinoNode(uint8_t id, uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, const char *ssid, const char *password);
        void loop();

    private:
        KLineKWP1281Lib *kLine;
		SoftwareSerial *softwareSerial;
		uint8_t pin_rx;
    	uint8_t pin_tx;
        Task *readValuesTask;
        Task *voltageReadingTask;
        void voltageCallback();
        KlineEcuEnum *lastConnectedEcu;
        AfterReadExecutors *afterReadExecutors;
        bool klineConnected;
        void readValues();
};
