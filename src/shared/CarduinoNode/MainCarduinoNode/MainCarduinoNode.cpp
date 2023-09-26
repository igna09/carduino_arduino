#include "MainCarduinoNode.h"
#include "../../executors/Executors.h"

MainCarduinoNode::MainCarduinoNode(int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(cs, interruptPin, ssid, password) {
    this->scheduler = new Scheduler();
    this->aht = new Adafruit_AHTX0();
    this->aht->begin();

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), scheduler, true);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), scheduler, true);

    this->scheduler->startNow();

    this->executors->addExecutor(new CarstatusExecutor());
    // this->executors->addExecutor(new AllMessageExecutor());
    // this->executors->addExecutor(new WriteSettingExecutor());
};

void MainCarduinoNode::luminanceCallback() {
    float volts = analogRead(A0) * 3.3 / 1024.0;
    float amps = volts / 10000.0; // across 10,000 Ohms
    float microamps = amps * 1000000;
    // 0 --> 1000 lux
    int lux = microamps * 2;
    
    CarstatusMessage m(&Carstatus::INTERNAL_LUMINANCE, lux);
    sendCanbusMessage(m);
};

void MainCarduinoNode::temperatureCallback() {
    sensors_event_t humidity, temp;
    this->aht->getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    
    CarstatusMessage m(&Carstatus::INTERNAL_TEMPERATURE, temp.temperature);
    sendCanbusMessage(m);
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
        String s0 = s.substring(0, s.indexOf(";"));
        Category *c = (Category*)Category::getValueByName((char*)s0.c_str());
        String s1 = s.substring(s.indexOf(";"));
        String s3 = s1.substring(0,s1.indexOf("-"));
        const CategoryToEnums *cte = CategoryToEnums::getValueByCategory(c);
        cte->enumNameToCategoryFunction((char*)s3.c_str());

        CanbusMessage m;
        //TODO: add s management
        manageReceivedUsbMessage(m);
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
