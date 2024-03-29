#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(id, cs, interruptPin, ssid, password, true, false) {
    this->aht = new Adafruit_AHTX0();
    this->aht->begin();

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), this->scheduler, true);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(30000, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), this->scheduler, true);

    this->canExecutors->addExecutor(new CarstatusExecutor());
    this->canExecutors->addExecutor(new MediaControlExecutor());
    this->canExecutors->addExecutor(new HeartbeatExecutor());
    // this->canExecutors->addExecutor(new ReadSettingExecutor());
    // this->canExecutors->addExecutor(new AllMessageExecutor());
    // this->canExecutors->addExecutor(new WriteSettingExecutor());

    this->usbExecutors = new Executors();
    this->usbExecutors->addExecutor(new WriteSettingExecutor());
    this->usbExecutors->addExecutor(new ReadSettingExecutor());

    this->lastReceivedHeartbeats = new std::map<uint8_t, unsigned long>();
};

void MainCarduinoNode::luminanceCallback() {
    float volts = analogRead(A0) * 3.3 / 1024.0;
    float amps = volts / 10000.0; // across 10,000 Ohms
    float microamps = amps * 1000000;
    // 0 --> 1000 lux
    int lux = microamps * 2;
    
    CarstatusMessage m(&Carstatus::INTERNAL_LUMINANCE, lux);
    sendCanbusMessage(&m);
    sendSerialMessage(&m);
};

void MainCarduinoNode::temperatureCallback() {
    sensors_event_t humidity, temp;
    this->aht->getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
    
    CarstatusMessage m(&Carstatus::INTERNAL_TEMPERATURE, temp.temperature);
    sendCanbusMessage(&m);
    sendSerialMessage(&m);
};

unsigned long int next = 0;

void MainCarduinoNode::loop() {
    CarduinoNode::loop();

    /**
     * manage received messages over USB
    */
    if(Serial.available() > 0) {
        String s = Serial.readStringUntil('\n');
        handleReceivedSerialMessage(s);
    }

    /*if(millis() > 5000 && !mockReceived) {
        mockReceived = true;
        handleReceivedSerialMessage("READ_SETTINGS;OTA_MODE;false;");
    }*/
}

void MainCarduinoNode::handleReceivedSerialMessage(String receivedMessage) {
    this->printlnWrapper("CarduinoNode::handleReceivedSerialMessage " + receivedMessage);
    SplittedUsbMessage *splittedUsbMessage = splitReceivedUsbMessage(receivedMessage);

    if(splittedUsbMessage->isValid) {
        const Category *c = (const Category*) Category::getValueByName((char*) splittedUsbMessage->messages[0].c_str());

        CanbusMessage *canbusMessage = nullptr;
        // TODO: replace with a factory
        if(c->getEnumFromNameFunction != nullptr) {
            const TypedEnum *typedEnumMessage = (const TypedEnum*) c->getEnumFromNameFunction((char*) splittedUsbMessage->messages[1].c_str());

            if(typedEnumMessage->type->id == CanbusMessageType::BOOL.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].equals("true")), 1);
            } else if(typedEnumMessage->type->id == CanbusMessageType::INT.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray((int) splittedUsbMessage->messages[2].toInt()), 4);
            } else if(typedEnumMessage->type->id == CanbusMessageType::FLOAT.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].toFloat()), 5);
            }
        } else {
            canbusMessage = new CanbusMessage(generateId(*c, 0), {}, 0);
        }

        if(canbusMessage != nullptr) {
            usbExecutors->execute(this, canbusMessage);
            delete canbusMessage;
        }
    }

    delete splittedUsbMessage;
}

void MainCarduinoNode::manageReceivedUsbMessage(CanbusMessage message) {
    sendByteCanbus(message.id, message.payloadLength, message.payload);
}

void MainCarduinoNode::sendSerialMessage(CanbusMessage *message) {
    Serial.println(message->toSerialString());
    Serial.flush();
}

SplittedUsbMessage* MainCarduinoNode::splitReceivedUsbMessage(String message) {
    SplittedUsbMessage *splittedUsbMessage = new SplittedUsbMessage();

    int i;
    splittedUsbMessage->isValid = true;
    for(i = 0; i < 3 && splittedUsbMessage->isValid; i++) {
        if(message.indexOf(";") >= 0) {
            splittedUsbMessage->messages[i] = message.substring(0, message.indexOf(";"));
            message = message.substring(message.indexOf(";") + 1);
        } else {
            splittedUsbMessage->isValid = false;
        }
    }

    return splittedUsbMessage;
}
