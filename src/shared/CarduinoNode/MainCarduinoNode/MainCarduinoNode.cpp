#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(id, cs, interruptPin, ssid, password, true, false) {
    this->addSetting(Setting::SWC_PAIR, false, [&](SettingInformation *settingInformation){
        if(settingInformation->valueType->boolValue) {
            this->startSwcPairing();
        }
    });
    
    this->aht = new Adafruit_AHTX0();
    this->aht->begin();

    this->pcfSwcSetup();
    this->pcfDigitalPinsSetup();

    this->lastPressedMillis = 0;
    this->isPressing = false;
    this->isPairing = false;
    this->isWaitingPairing = false;

    luminanceTask = new Task(1000, TASK_FOREVER, std::bind(&MainCarduinoNode::luminanceCallback, this), this->scheduler, true);
    temperatureTask = new Task(30000, TASK_FOREVER, std::bind(&MainCarduinoNode::temperatureCallback, this), this->scheduler, true);

    this->nodeInformations = new std::map<uint8_t, NodeInformation*>();

    this->canExecutors->addExecutor(new CarstatusExecutor());
    this->canExecutors->addExecutor(new MediaControlExecutor());
    this->canExecutors->addExecutor(new HeartbeatExecutor());
    this->canExecutors->addExecutor(new MainNodeCanReadSettingExecutor());
    this->canExecutors->addExecutor(new MainNodeCanEvent());

    this->usbExecutors = new Executors();
    this->usbExecutors->addExecutor(new WriteSettingExecutor());
    this->usbExecutors->addExecutor(new MainNodeSerialGetSettings());

    turnOffRadioTask = new Task(RADIO_TURN_OFF_TIMER, 1, std::bind(&MainCarduinoNode::turnOffSystem, this), this->scheduler, false);

    this->sendEvent(&Event::GET_HELLOS);
    
    this->isRadioOn = true;
    this->isKeyOn = true;
};

void MainCarduinoNode::luminanceCallback() {
    float volts = calculateVoltage(analogRead(A0) / 1024.0, WEMOS_D1_MINI_VOLTAGE_DIVIDER_R1, WEMOS_D1_MINI_VOLTAGE_DIVIDER_R2);
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
}

void MainCarduinoNode::handleReceivedSerialMessage(String receivedMessage) {
    this->printlnWrapper("MainCarduinoNode::handleReceivedSerialMessage " + receivedMessage);
    SplittedUsbMessage *splittedUsbMessage = splitReceivedUsbMessage(receivedMessage);

    if(splittedUsbMessage->isValid) {
        bool isNumericMode = isNumeric(splittedUsbMessage->messages[0]);

        const Category *c;

        if(isNumericMode) {
            c = (const Category*) Category::getValueById(splittedUsbMessage->messages[0].toInt());
        } else {
            c = (const Category*) Category::getValueByName((char*) splittedUsbMessage->messages[0].c_str());
        }

        CanbusMessage *canbusMessage = nullptr;
        // TODO: replace with a factory
        if(c->getEnumFromNameFunction != nullptr && c->getEnumFromIdFunction != nullptr) {
            const TypedEnum *typedEnumMessage;

            if(isNumericMode) {
                typedEnumMessage = (const TypedEnum*) c->getEnumFromIdFunction(splittedUsbMessage->messages[1].toInt());
            } else {
                typedEnumMessage = (const TypedEnum*) c->getEnumFromNameFunction((char*) splittedUsbMessage->messages[1].c_str());
            }

            if(typedEnumMessage != nullptr) {
                if(typedEnumMessage->type->id == CanbusMessageType::BOOL.id) {
                    canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].equals("TRUE")), 1);
                } else if(typedEnumMessage->type->id == CanbusMessageType::INT.id) {
                    canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray((int) splittedUsbMessage->messages[2].toInt()), 4);
                } else if(typedEnumMessage->type->id == CanbusMessageType::FLOAT.id) {
                    canbusMessage = new CanbusMessage(generateId(*c, *typedEnumMessage), convertValueToByteArray(splittedUsbMessage->messages[2].toFloat()), 5);
                }
            }
        } else {
            // this->printlnWrapper("CarduinoNode::handleReceivedSerialMessage is nullptr");
            canbusMessage = new TypedCanbusMessage(generateId(*c, 0), false);
            // uint8_t value[1] = {0};
            // canbusMessage = new CanbusMessage(generateId(*c, 0), value, 1);
            // this->printlnWrapper("CarduinoNode::handleReceivedSerialMessage created message");
        }

        if(canbusMessage != nullptr) {
            usbExecutors->execute(this, canbusMessage);
            delete canbusMessage;
        }
    } else {
        this->printlnWrapper("MainCarduinoNode::handleReceivedSerialMessage malformed message " + receivedMessage);
    }

    delete splittedUsbMessage;
}

void MainCarduinoNode::manageReceivedUsbMessage(CanbusMessage message) {
    sendByteCanbus(message.id, message.payloadLength, message.payload);
}

void MainCarduinoNode::sendSerialMessage(CanbusMessage *message) {
    printlnWrapper("MainCarduinoNode::sendSerialMessage " + message->toSerialHumanString());
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
        this->pcf8574Swc->digitalWrite(mediaControl->pin, LOW);
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
            this->pcf8574Swc->digitalWrite(this->pressedPin, HIGH);
            this->isPressing = false;
        }
    } else if(this->isPairing) {
        if(millis() > this->lastPressedMillis + SWC_PAIRING_INTERVAL) {
            this->pcf8574Swc->digitalWrite(this->pressedPin, HIGH);
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
            this->pcf8574Swc->digitalWrite(this->pressedPin, LOW);
            this->lastPressedMillis = millis();
            this->isWaitingPairing = false;
            this->isPairing = true;
            this->printlnWrapper("stop waiting and start pressing PIN " + String(this->pressedPin) + " " + String(millis()));
        }
    }
}

void MainCarduinoNode::turnOffSystem() {
    printlnWrapper("MainCarduinoNode::turnOffSystem");
    //TODO: when all turn off events happened (i receive TURN_OFF_COMPLETE from important nodes like door one) turn off also the radio??
    this->isRadioOn = false;
    this->pcf8574DigitalPins->digitalWrite(RADIO_POWER_MOSFET_PIN, LOW);
}

// void MainCarduinoNode::manageRadioPower() {
//     PinInformation *pinInformation = this->getPinInformation(ACCESSORY_12_V_PIN);
//     /**
//      * read digital input +12v ACC line to manage events
//     */
//     if(!pinInformation->isHigh && this->isKeyOn) {
//         this->isKeyOn = false;
//         // this->turnOffRadioTask-> // reset remaining timer
//         this->turnOffRadioTask->restartDelayed();
//         this->sendEvent(&Event::TURN_OFF);
//     } else if(!pinInformation->isHigh && this->turnOffRadioTask->isEnabled()) {
//         this->turnOffRadioTask->disable();
//         this->isKeyOn = true;
//         this->sendEvent(&Event::TURN_OFF_INTERRUPT);
//     }
// }

void MainCarduinoNode::pcfSwcSetup() {
    this->pcf8574Swc = new PCF8574(0x20);

	this->pcf8574Swc->pinMode(P0, OUTPUT);
    this->pcf8574Swc->pinMode(P1, OUTPUT);
    this->pcf8574Swc->pinMode(P2, OUTPUT);
    this->pcf8574Swc->pinMode(P3, OUTPUT);
    this->pcf8574Swc->pinMode(P4, OUTPUT);
    this->pcf8574Swc->pinMode(P5, OUTPUT);
    this->pcf8574Swc->pinMode(P6, OUTPUT);
    this->pcf8574Swc->pinMode(P7, OUTPUT);

    this->pcf8574Swc->begin();

    this->pcf8574Swc->digitalWrite(P0, HIGH);
    this->pcf8574Swc->digitalWrite(P1, HIGH);
    this->pcf8574Swc->digitalWrite(P2, HIGH);
    this->pcf8574Swc->digitalWrite(P3, HIGH);
    this->pcf8574Swc->digitalWrite(P4, HIGH);
    this->pcf8574Swc->digitalWrite(P5, HIGH);
    this->pcf8574Swc->digitalWrite(P6, HIGH);
    this->pcf8574Swc->digitalWrite(P7, HIGH);
}

void MainCarduinoNode::pcfDigitalPinsSetup() {
    this->pcf8574DigitalPins = new PCF8574(0x21);

	this->pcf8574DigitalPins->pinMode(RADIO_POWER_MOSFET_PIN, OUTPUT);
    this->pcf8574DigitalPins->pinMode(ACCESSORY_12_V_PIN, INPUT);

    this->pcf8574DigitalPins->begin();

    this->pcf8574DigitalPins->digitalWrite(RADIO_POWER_MOSFET_PIN, HIGH);

    this->addPinToRead(ACCESSORY_12_V_PIN, this->pcf8574DigitalPins, [&](PinInformation *pinInformation){
        printlnWrapper("ACCESSORY_12_V_PIN changed from " + String(!pinInformation->isHigh) + " to " + String(pinInformation->isHigh));
        this->isKeyOn = !pinInformation->isHigh;
        //PIN STATE CHANGED
        if(!this->isKeyOn) {
            // this->turnOffRadioTask-> // reset remaining timer
            this->turnOffRadioTask->restartDelayed();
            this->sendEvent(&Event::TURN_OFF);
        } else if(this->isKeyOn && this->turnOffRadioTask->isEnabled()) {
            this->turnOffRadioTask->disable();
            this->sendEvent(&Event::TURN_OFF_INTERRUPT);
        }
    });
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
        nodeInformation = new NodeInformation();
        nodeInformation->id = id;
        nodeInformation->lastCompletedEvent = nullptr;
        nodeInformation->lastTimeReceivedHeartBeat = 0;

        (*this->nodeInformations)[id] = nodeInformation;
    }

    return nodeInformation;
}
