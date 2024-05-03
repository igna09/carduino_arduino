#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(id, cs, interruptPin, ssid, password, true, false) {
    this->aht = new Adafruit_AHTX0();
    this->aht->begin();

    this->pcfSetup();

    this->lastPressedMillis = 0;
    this->isPressing = false;
    this->isPairing = false;
    this->isWaitingPairing = false;

    LuminanceCallback<void(void)>::func = std::bind(&MainCarduinoNode::luminanceCallback, this);
    luminanceTask = new Task(1000, TASK_FOREVER, static_cast<TaskCallback>(LuminanceCallback<void(void)>::callback), this->scheduler, true);

    TemperatureCallback<void(void)>::func = std::bind(&MainCarduinoNode::temperatureCallback, this);
    temperatureTask = new Task(30000, TASK_FOREVER, static_cast<TaskCallback>(TemperatureCallback<void(void)>::callback), this->scheduler, true);

    this->nodeInformations = new std::map<uint8_t, NodeInformation*>();

    this->canExecutors->addExecutor(new CarstatusExecutor());
    this->canExecutors->addExecutor(new MediaControlExecutor());
    this->canExecutors->addExecutor(new HeartbeatExecutor());
    this->canExecutors->addExecutor(new MainNodeCanReadSettingExecutor());
    this->canExecutors->addExecutor(new MainNodeCanHello());

    this->usbExecutors = new Executors();
    this->usbExecutors->addExecutor(new WriteSettingExecutor());
    this->usbExecutors->addExecutor(new MainNodeSerialGetSettings());

    TurnOffRadioCallback<void(void)>::func = std::bind(&MainCarduinoNode::startTurnOffSystem, this);
    turnOffRadioTask = new Task(RADIO_TURN_OFF_TIMER, 1, static_cast<TaskCallback>(TurnOffRadioCallback<void(void)>::callback), this->scheduler, false);

    this->sendEvent(&Event::GET_HELLOS);

    //turn ON MOSFET on remote line
    //pinMode(pin, OUT);
    //digitalWrite(pin, HIGH);
    this->isRadioOn = true;
    this->isKeyOn = true;
    this->sendEvent(&Event::TURN_ON);
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

    manageSwc();

    manageRadioPower();
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

            if(typedEnumMessage != nullptr) {
            if(typedEnumMessage->type->id == CanbusMessageType::BOOL.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].equals("true")), 1);
            } else if(typedEnumMessage->type->id == CanbusMessageType::INT.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray((int) splittedUsbMessage->messages[2].toInt()), 4);
            } else if(typedEnumMessage->type->id == CanbusMessageType::FLOAT.id) {
                canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].toFloat()), 5);
                }
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

void MainCarduinoNode::executeSwcCommand(MediaControl *mediaControl) {
    if(!this->isPressing && mediaControl->pin != 255) {
        this->isPressing = true;
        this->pressedPin = mediaControl->pin;
        this->pcf8574->digitalWrite(mediaControl->pin, LOW);
        this->lastPressedMillis = millis();
    }
}

void MainCarduinoNode::startSwcPairing() {
    this->isWaitingPairing = true;
    this->pressedPin = 0;
    this->lastPressedMillis = millis();

    this->printlnWrapper("Start waiting PIN " + String(this->pressedPin) + " " + String(millis()));
}

void MainCarduinoNode::manageSwc() {
    if(this->isPressing) {
        if(millis() > this->lastPressedMillis + SWC_PRESS_INTERVAL) {
            this->pcf8574->digitalWrite(this->pressedPin, HIGH);
            this->isPressing = false;
        }
    } else if(this->isPairing) {
        if(millis() > this->lastPressedMillis + SWC_PAIRING_INTERVAL) {
            this->pcf8574->digitalWrite(this->pressedPin, HIGH);
            this->printlnWrapper("Stop pressing PIN " + String(this->pressedPin) + " " + String(millis()));
            this->isPairing = false;
            if(this->pressedPin < SWC_PIN_SIZE - 1) {
                this->pressedPin++;
                this->isWaitingPairing = true;
                this->lastPressedMillis = millis();

                this->printlnWrapper("Start waiting PIN " + String(this->pressedPin) + " " + String(millis()));
            }
        }
    } else if(this->isWaitingPairing) {
        int intervalToWait;
        if(this->pressedPin == 0) {
            intervalToWait = SWC_FIRST_WAITING_PAIRING_INTERVAL;
        } else {
            intervalToWait = SWC_WAITING_PAIRING_INTERVAL;
        }

        if(millis() > this->lastPressedMillis + intervalToWait) {
            this->pcf8574->digitalWrite(this->pressedPin, LOW);
            this->lastPressedMillis = millis();
            this->isWaitingPairing = false;
            this->isPairing = true;
            this->printlnWrapper("stop waiting and start pressing PIN " + String(this->pressedPin) + " " + String(millis()));
        }
    }
}

void MainCarduinoNode::startTurnOffSystem() {
    this->sendEvent(&Event::TURN_OFF);

    //when all turn off events happened (i receive TURN_OFF_COMPLETE from important nodes like door one) turn off also the radio
    this->isRadioOn = false;
    // digitalWrite(pin, LOW);
}

void MainCarduinoNode::manageRadioPower() {
    /**
     * read digital input +12v ACC line to manage events
    */
    if(/*digitalRead(pin) == LOW*/true && this->isKeyOn) {
        this->isKeyOn = false;
        // this->turnOffRadioTask-> // reset remaining timer
        this->turnOffRadioTask->enable();
    }
    
    if(this->turnOffRadioTask->isEnabled() && /*digitalRead(pin) == HIGH*/true) {
        this->turnOffRadioTask->disable();
    }
}

void MainCarduinoNode::pcfSetup() {
    this->pcf8574 = new PCF8574(0x20);

	this->pcf8574->pinMode(P0, OUTPUT);
    this->pcf8574->pinMode(P1, OUTPUT);
    this->pcf8574->pinMode(P2, OUTPUT);
    this->pcf8574->pinMode(P3, OUTPUT);
    this->pcf8574->pinMode(P4, OUTPUT);
    this->pcf8574->pinMode(P5, OUTPUT);
    this->pcf8574->pinMode(P6, OUTPUT);
    this->pcf8574->pinMode(P7, OUTPUT);

    this->pcf8574->begin();

    this->pcf8574->digitalWrite(P0, HIGH);
    this->pcf8574->digitalWrite(P1, HIGH);
    this->pcf8574->digitalWrite(P2, HIGH);
    this->pcf8574->digitalWrite(P3, HIGH);
    this->pcf8574->digitalWrite(P4, HIGH);
    this->pcf8574->digitalWrite(P5, HIGH);
    this->pcf8574->digitalWrite(P6, HIGH);
    this->pcf8574->digitalWrite(P7, HIGH);
}

NodeInformation* MainCarduinoNode::getNodeInformation(uint8_t id) {
    std::map<uint8_t, NodeInformation*>::iterator it = this->nodeInformations->find(id);
    if (it != this->nodeInformations->end()) {
        return it->second;
    } else {
        return nullptr;
    }
}

NodeInformation* MainCarduinoNode::createOrGetNodeInformation(uint8_t id) {
    NodeInformation *nodeInformation = this->getNodeInformation(id);

    if (nodeInformation == nullptr) {
        NodeInformation *nodeInformation = new NodeInformation();
        nodeInformation->id = id;
        nodeInformation->lastCompletedEvent = nullptr;
        nodeInformation->lastTimeReceivedHeartBeat = 0;

        (*this->nodeInformations)[id] = nodeInformation;
    }

    return nodeInformation;
}
