#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(int cs, int interruptPin, String ssid, String password) : CarduinoNode(cs, interruptPin, ssid, password) {
    this->scheduler = new Scheduler();
    this->aht = new Adafruit_AHTX0();

    Callback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    TaskCallback func = static_cast<TaskCallback>(Callback<void(void)>::callback);
    Task weatherTask(TASK_MILLISECOND, TASK_FOREVER, func, scheduler, true);

};
MainCarduinoNode::~MainCarduinoNode() {};

void MainCarduinoNode::luminanceCallback() {
    float volts = analogRead(A0) * 5.0 / 1024.0;
    float amps = volts / 10000.0; // across 10,000 Ohms
    float microamps = amps * 1000000;
    float lux = microamps * 2.0;

    sendIntMessageCanbus(0, floor(lux));
};

void MainCarduinoNode::temperatureCallback() {
    sensors_event_t humidity, temp;
    this->aht->getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

    sendFloatMessageCanbus(0, temp.temperature);
};

void MainCarduinoNode::loop() {
    CarduinoNode::loop();
    this->scheduler->execute();
}

void MainCarduinoNode::manageReceivedMessage(CanbusMessage message) {

}
