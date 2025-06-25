#include "MainCarduinoNode.h"

MainCarduinoNode::MainCarduinoNode(uint8_t id, int cs, int interruptPin, char *ssid, char *password) : CarduinoNode(id, cs, interruptPin, ssid, password, true, false, false) {
    this->enable();

	this->addSetting(&Setting::SEND_ALL_MESSAGES_TO_RADIO, false, nullptr, true);
    this->restoreSettings();

    heartbeatWdtTask->disable();
    
    this->aht = new Adafruit_AHTX0();
    this->aht->begin();

    // this->pcfSwcSetup();
    this->pcfDigitalPinsSetup();

    this->lastPressedMillis = 0;
    this->isPressing = false;
    this->isPairing = false;
    this->isWaitingPairing = false;

    luminanceTask = new Task(1000, TASK_FOREVER, std::bind(&MainCarduinoNode::luminanceCallback, this), this->scheduler, true);
    temperatureTask = new Task(30000, TASK_FOREVER, std::bind(&MainCarduinoNode::temperatureCallback, this), this->scheduler, true);

    this->nodeInformations = new std::map<uint8_t, NodeInformation*>();

    this->canExecutor->addExecutor(new CarstatusExecutor());
    this->canExecutor->addExecutor(new MediaControlExecutor());
    this->canExecutor->addExecutor(new HeartbeatExecutor());
    this->canExecutor->addExecutor(new MainNodeCanReadSettingExecutor());
    this->canExecutor->addExecutor(new MainNodeCanEvent());
    this->canExecutor->addExecutor(new MainNodeCanLog());
    this->canExecutor->addExecutor(new AllMessageExecutor());

    // this->usbExecutor->addExecutor(new MainCarduinoNodeSerialEvent());

    turnOffRadioTask = new Task(RADIO_TURN_OFF_TIMER, 1, std::bind(&MainCarduinoNode::turnOffSystem, this), this->scheduler, false);
    
    this->isRadioOn = true;
    this->isKeyOn = true;

    this->sendEvent(&Event::GET_HELLOS, ALL_NODES);
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

void MainCarduinoNode::loop() {
    CarduinoNode::loop();

    /*if(millis() > 5000 && !mockReceived) {
        mockReceived = true;
        handleReceivedSerialMessage("READ_SETTINGS;OTA_MODE;false;");
    }*/

    // manageSwc();
}

// void MainCarduinoNode::executeSwcCommand(MediaControl *mediaControl) {
//     if(!this->isPressing && mediaControl->pin != 255) {
//         this->isPressing = true;
//         this->pressedPin = mediaControl->pin;
//         this->pcf8574Swc->digitalWrite(mediaControl->pin, LOW);
//         this->lastPressedMillis = millis();
//     }
// }

// void MainCarduinoNode::startSwcPairing() {
//     this->isWaitingPairing = true;
//     this->pressedPin = 0;
//     this->lastPressedMillis = millis();

//     this->printlnWrapper("Start waiting PIN " + String(this->pressedPin) + " " + String(millis()));
// }

// void MainCarduinoNode::manageSwc() {
//     if(this->isPressing) {
//         if(millis() > this->lastPressedMillis + SWC_PRESS_INTERVAL) {
//             this->pcf8574Swc->digitalWrite(this->pressedPin, HIGH);
//             this->isPressing = false;
//         }
//     } else if(this->isPairing) {
//         if(millis() > this->lastPressedMillis + SWC_PAIRING_INTERVAL) {
//             this->pcf8574Swc->digitalWrite(this->pressedPin, HIGH);
//             this->printlnWrapper("Stop pressing PIN " + String(this->pressedPin) + " " + String(millis()));
//             this->isPairing = false;
//             if(this->pressedPin < SWC_PIN_SIZE - 1) {
//                 this->pressedPin++;
//                 this->isWaitingPairing = true;
//                 this->lastPressedMillis = millis();

//                 this->printlnWrapper("Start waiting PIN " + String(this->pressedPin) + " " + String(millis()));
//             }
//         }
//     } else if(this->isWaitingPairing) {
//         int intervalToWait;
//         if(this->pressedPin == 0) {
//             intervalToWait = SWC_FIRST_WAITING_PAIRING_INTERVAL;
//         } else {
//             intervalToWait = SWC_WAITING_PAIRING_INTERVAL;
//         }

//         if(millis() > this->lastPressedMillis + intervalToWait) {
//             this->pcf8574Swc->digitalWrite(this->pressedPin, LOW);
//             this->lastPressedMillis = millis();
//             this->isWaitingPairing = false;
//             this->isPairing = true;
//             this->printlnWrapper("stop waiting and start pressing PIN " + String(this->pressedPin) + " " + String(millis()));
//         }
//     }
// }

void MainCarduinoNode::turnOffSystem() {
    printlnWrapper("MainCarduinoNode::turnOffSystem");
    //TODO: when all turn off events happened (i receive TURN_OFF_COMPLETE from important nodes like door one) turn off also the radio??
    this->isRadioOn = false;
    this->pcf8574DigitalPins->digitalWrite(RADIO_POWER_MOSFET_PIN, LOW);
}

/*void MainCarduinoNode::manageRadioPower() {
    PinInformation *pinInformation = this->getPinInformation(ACCESSORY_12_V_PIN);
    /**
     * read digital input +12v ACC line to manage events
    *
    if(!pinInformation->isHigh && this->isKeyOn) {
        this->isKeyOn = false;
        // this->turnOffRadioTask-> // reset remaining timer
        this->sendEvent(&Event::DISABLE);
        this->turnOffRadioTask->restartDelayed();
    } else if(!pinInformation->isHigh && this->turnOffRadioTask->isEnabled()) {
        this->turnOffRadioTask->disable();
        this->isKeyOn = true;
        this->sendEvent(&Event::DISABLE_INTERRUPT);
    }
}*/

// void MainCarduinoNode::pcfSwcSetup() {
//     this->pcf8574Swc = new PCF8574(0x20, NODE_SDA, NODE_SCL);

// 	this->pcf8574Swc->pinMode(P0, OUTPUT);
//     this->pcf8574Swc->pinMode(P1, OUTPUT);
//     this->pcf8574Swc->pinMode(P2, OUTPUT);
//     this->pcf8574Swc->pinMode(P3, OUTPUT);
//     this->pcf8574Swc->pinMode(P4, OUTPUT);
//     this->pcf8574Swc->pinMode(P5, OUTPUT);
//     this->pcf8574Swc->pinMode(P6, OUTPUT);
//     this->pcf8574Swc->pinMode(P7, OUTPUT);

//     bool i2cValid = this->pcf8574Swc->begin();

//     if(i2cValid) {
//         this->pcf8574Swc->digitalWrite(P0, HIGH);
//         this->pcf8574Swc->digitalWrite(P1, HIGH);
//         this->pcf8574Swc->digitalWrite(P2, HIGH);
//         this->pcf8574Swc->digitalWrite(P3, HIGH);
//         this->pcf8574Swc->digitalWrite(P4, HIGH);
//         this->pcf8574Swc->digitalWrite(P5, HIGH);
//         this->pcf8574Swc->digitalWrite(P6, HIGH);
//         this->pcf8574Swc->digitalWrite(P7, HIGH);
//     }
// }

void MainCarduinoNode::pcfDigitalPinsSetup() {
    this->pcf8574DigitalPins = new PCF8574(0x21, NODE_SDA, NODE_SCL);

	this->pcf8574DigitalPins->pinMode(RADIO_POWER_MOSFET_PIN, OUTPUT);
    this->pcf8574DigitalPins->pinMode(ACCESSORY_12_V_PIN, INPUT);

    bool i2cValid = this->pcf8574DigitalPins->begin();

    printlnWrapper("I2C " + String(i2cValid ? "" : "not") + " valid");

    if(i2cValid) {
        this->pcf8574DigitalPins->digitalWrite(RADIO_POWER_MOSFET_PIN, HIGH);

        this->addPinToRead(ACCESSORY_12_V_PIN, this->pcf8574DigitalPins, [&](PinInformation *pinInformation){
            printlnWrapper("ACCESSORY_12_V_PIN changed from " + String(!pinInformation->isHigh) + " to " + String(pinInformation->isHigh));
            this->isKeyOn = !pinInformation->isHigh;
            //PIN STATE CHANGED
            if(!this->isKeyOn) {
            // Serial.println("key off");
                delayTask(2500, [&](){
            // Serial.println("delayed");
                    PinInformation* pin = getPinInformation(ACCESSORY_12_V_PIN);
                    if(pin != nullptr) {
                        bool newKeyIsOn = !pin->isHigh;
                        if(!newKeyIsOn) {
            // Serial.println("delayed key off");
                            // this->turnOffRadioTask-> // reset remaining timer
                            this->turnOffRadioTask->restartDelayed();
                            this->sendEvent(&Event::DISABLE, ALL_NODES);
                        }else if(newKeyIsOn) {
            // Serial.println("delayed key on");
                            this->turnOffRadioTask->disable();
                            this->sendEvent(&Event::DISABLE_INTERRUPT, ALL_NODES);
                        } else {
                            // Serial.println("delayed no action");
                        }
                    }
                });
            } else if(this->isKeyOn && this->turnOffRadioTask->isEnabled()) {
                this->turnOffRadioTask->disable();
                this->sendEvent(&Event::DISABLE_INTERRUPT, ALL_NODES);
            }
        });
    }
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

void MainCarduinoNode::sendLog(uint8_t nodeId, bool value) {
    LogMessage *logMessage = new LogMessage(this->id, nodeId, value);
	this->sendSerialMessage(logMessage);
    delete logMessage;
}

void MainCarduinoNode::sendLog(uint8_t nodeId, float value) {
    LogMessage *logMessage = new LogMessage(this->id, nodeId, value);
	this->sendSerialMessage(logMessage);
    delete logMessage;
}

void MainCarduinoNode::sendLog(uint8_t nodeId, int value) {
    LogMessage *logMessage = new LogMessage(this->id, nodeId, value);
	this->sendSerialMessage(logMessage);
    delete logMessage;
}

void MainCarduinoNode::test() {
    EventMessage m(&Event::BLE_PAIRING_CODE, 123456);
    sendSerialMessage(&m);
}

void MainCarduinoNode::onOnlineEvent(OnlineEnum event) {
    CarduinoNode::onOnlineEvent(event);

    if(event == OnlineEnum::ONLINE) {
        
    }
}
