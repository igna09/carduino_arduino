#include "MainCarduinoNode.h"
#include "../../executors/Executors.h"

MainCarduinoNode::MainCarduinoNode(int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(cs, interruptPin, ssid, password) {
    this->scheduler = new Scheduler();
    this->aht = new Adafruit_AHTX0();

    //TODO: verify i can use only one callback

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), scheduler, false);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), scheduler, true);

    this->scheduler->startNow();

    this->executors->addExecutor(new CarstatusExecutor());
    // this->executors->addExecutor(new WriteSettingExecutor());
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
    //Serial.println(generateId(Category::READ_SETTINGS, Carstatus::INTERNAL_TEMPERATURE), BIN);
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
        //TODO: add s management
        manageReceivedUsbMessage(m);
    }
        
    // TODO: to remove, just for testing purpose
    if(millis() > next) {
        next = random((1 * 1000),(2 * 1000)) + millis();
        
        uint8_t payload[] = {0x00, 0x00, 0x00, 0x10, 0x02};
        CanbusMessage *m = new CanbusMessage(generateId(Category::CAR_STATUS, Carstatus::INTERNAL_TEMPERATURE), payload, 5);
        manageReceivedCanbusMessage(m);
        delete m;
    }
}

// void MainCarduinoNode::manageReceivedCanbusMessage(CanbusMessage message) {
//     CarduinoNode::manageReceivedCanbusMessage(message);
//     this->sendSerialMessage(message);
// }

void MainCarduinoNode::manageReceivedUsbMessage(CanbusMessage message) {
    sendByteCanbus(message.id, message.payloadLength, message.payload);
}

void MainCarduinoNode::sendSerialMessage(CanbusMessage *message) {
    Serial.println(message->toSerialString());
}
