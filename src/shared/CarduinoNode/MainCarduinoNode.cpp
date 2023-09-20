#include "./shared/CanbusMessage/CarstatusCanbusMessage/CarstatusCanbusMessage.h"
#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(cs, interruptPin, ssid, password) {
    this->scheduler = new Scheduler();
    this->aht = new Adafruit_AHTX0();

    //TODO: verify i can use only one callback

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(TASK_MILLISECOND, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), scheduler, false);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(TASK_MILLISECOND, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), scheduler, false);

    this->scheduler->startNow();
};

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

unsigned long int next = 0;

void MainCarduinoNode::loop() {
    CarduinoNode::loop();
    this->scheduler->execute();
        
    if(millis() > next) {
        next = random((5 * 1000),(10 * 1000)) + millis();
        
        uint8_t payload[] = {0x00, 0x00, 0x00, 0x10, 0x02};
        CanbusMessage m((unsigned long)0b00100000001, payload, (uint8_t)5);
        manageReceivedMessage(m);
    }
}

void MainCarduinoNode::manageReceivedMessage(CanbusMessage message) {
    CarstatusCanbusMessageTypedInterface *carstatusCanbusMessage = CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(message);
    Serial.println(*(carstatusCanbusMessage->toSerialString()));
    Serial.println(ESP.getFreeHeap());
}
