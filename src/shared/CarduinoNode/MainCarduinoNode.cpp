#include "./shared/CanbusMessage/CarstatusCanbusMessage/CarstatusCanbusMessage.h"
#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(cs, interruptPin, ssid, password) {
    this->scheduler = new Scheduler();
    this->aht = new Adafruit_AHTX0();

    //TODO: verify i can use only one callback

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), scheduler, false);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), scheduler, true);

    this->scheduler->startNow();
};

void MainCarduinoNode::luminanceCallback() {
    float volts = analogRead(A0) * 5.0 / 1024.0;
    float amps = volts / 10000.0; // across 10,000 Ohms
    float microamps = amps * 1000000;
    float lux = microamps * 2.0;

    Serial.println(lux);
    // sendIntMessageCanbus(0, floor(lux));
};

void MainCarduinoNode::temperatureCallback() {
    /*sensors_event_t humidity, temp;
    this->aht->getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data

    sendFloatMessageCanbus(0, temp.temperature);*/
    Serial.println(generateId(Category::READ_SETTINGS, Carstatus::INTERNAL_TEMPERATURE), BIN);
    // sendFloatMessageCanbus(generateId(Category::READ_SETTINGS, Carstatus::INTERNAL_TEMPERATURE), 24.5);
};

unsigned long int next = 0;

void MainCarduinoNode::loop() {
    CarduinoNode::loop();

    this->scheduler->execute();

    /**
     * manage received messages over USB
    */
    if(Serial.available() > 0) {
        String s = Serial.readStringUntil('\n');
        CanbusMessage m;
        manageReceivedUsbMessage(m);
    }

    /**
     * manage received messages over CANBUS
    */
    if (availableCanbusMessages()) {
      // iterate over all pending messages
      // If either the bus is saturated or the MCU is busy,
      // both RX buffers may be in use and reading a single
      // message does not clear the IRQ conditon.
      while (CAN_MSGAVAIL == can->checkReceive()) {
        uint8_t len = 0;
        uint8_t buf[8];
        long unsigned int id;

        can->readMsgBuf(&id, &len, buf);
        CanbusMessage m(id, buf, len);

        manageReceivedCanbusMessage(m);
      }
   }
        
    // TODO: to remove, just for testing purpose
    if(millis() > next) {
        next = random((1 * 1000),(2 * 1000)) + millis();
        
        uint8_t payload[] = {0x00, 0x00, 0x00, 0x10, 0x02};
        CanbusMessage m((unsigned long)0b00000000001, payload, (uint8_t)5);
        manageReceivedCanbusMessage(m);
    }
}

// TODO: manage received message
void MainCarduinoNode::manageReceivedCanbusMessage(CanbusMessage message) {
    CarstatusCanbusMessageTypedInterface *carstatusCanbusMessage = CarstatusCanbusMessageFactory::getCarstatusCanbusMessage(message);
    String s = carstatusCanbusMessage->toSerialString();
    Serial.println(s);
    delete carstatusCanbusMessage;
}

void MainCarduinoNode::manageReceivedUsbMessage(CanbusMessage message) {
    sendMessageCanBus(message.id, message.payloadLength, message.payload);
}
