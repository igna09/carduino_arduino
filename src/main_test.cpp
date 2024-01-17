#include <Arduino.h>
#include "shared/CarduinoNode/CarduinoNode/CarduinoNode.h"
#include "shared/CarduinoNode/KlineCarduinoNode/AfterReadExecutor/AfterReadExecutors.h"
#include "shared/CarduinoNode/KlineCarduinoNode/AfterReadExecutor/AfterReadExecutorInterface.h"
#include "./shared/SharedDefinitions.h"

class TestExecutor : public AfterReadExecutorInterface {
	public:
		void execute(CarduinoNode *node) {
			Serial.println("test");
		}
};

void setup()
{
	Serial.begin(BAUD_RATE);

	AfterReadExecutors *executors = new AfterReadExecutors();
	executors->addExecutor(new TestExecutor());

	executors->execute(new CarduinoNode(0xFF, D3, D0, "SSID_KLINE_CARDUINO_NODE", "pwd12345", false, false));
}

void loop()
{
	delay(1000);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
