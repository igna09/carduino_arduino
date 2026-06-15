#include "KlineCarduinoNode.h"

KlineCarduinoNode::KlineCarduinoNode(uint8_t id, uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, false, true) {
    this->restoreSettings();

	// const uint32_t myMask = 0b11100000000;         // where to look at
    // const uint32_t getSettingsFilter = 0b11100000000;       // what to find
    // can->init_Mask(0, 0, myMask);                // Init first mask
    // can->init_Filt(0, 0, getSettingsFilter);              // Init first filter
    // const uint32_t writeSettingFilter = 0b01100000000;       // what to find
    // can->init_Mask(1, 0, myMask);                // Init second mask - must be set, otherwise first mask isn't working
    // can->init_Filt(1, 0, writeSettingFilter);              // Init second filter...

	heartbeatWdtTask->disable();
	
	this->pin_rx = pin_rx;
    this->pin_tx = pin_tx;

	#if !defined(ARDUINO_ARCH_ESP32)
	this->softwareSerial = new SoftwareSerial(pin_rx, pin_tx);
	#endif

	std::function<void(unsigned long)> beginFunction = [&](unsigned long baud){
		#if defined(ARDUINO_ARCH_ESP32)
		Serial1.begin(baud, SERIAL_8N1, this->pin_rx, this->pin_tx);
		#else
		this->softwareSerial->begin(baud);
		#endif
	};
	std::function<void()> endFunction = [&](){
		#if defined(ARDUINO_ARCH_ESP32)
		Serial1.end();
		#else
		this->softwareSerial->end();
		#endif
	};
	std::function<void(uint8_t)> sendFunction = [&](uint8_t data){
		#ifdef DEBUG_KLINE_NODE
		this->printlnWrapper("K-OUT > 0x" + String(data, HEX));
		#endif
		#if defined(ARDUINO_ARCH_ESP32)
		Serial1.write(data);
		#else
		this->softwareSerial->write(data);
		#endif
	};
	std::function<bool(uint8_t&)> receiveFunction = [&](uint8_t &data){
		#if defined(ARDUINO_ARCH_ESP32)
		if (Serial1.available()) {
			data = Serial1.read();
		#else
		if (this->softwareSerial->available()) {
			data = this->softwareSerial->read();
		#endif
			#ifdef DEBUG_KLINE_NODE
			this->printlnWrapper("K-IN  < 0x" + String(data, HEX));
			#endif
			return true;
		}
		return false;
	};
	
    this->kLine = new KLineKWP1281Lib(beginFunction, endFunction, sendFunction, receiveFunction, pin_tx, true);
	this->lastConnectedEcu = nullptr;

	/**
	 * check if overhead is for use of std::Function
	*/
	// KlineCallback<void(void)>::func = std::bind(&KlineCarduinoNode::readValues, this);
    // readValuesTask = new Task(500, TASK_FOREVER, static_cast<TaskCallback>(KlineCallback<void(void)>::callback), scheduler, true);
    readValuesTask = new Task(500, TASK_FOREVER, std::bind(&KlineCarduinoNode::readValues, this), scheduler, true);
    this->voltageReadingTask = new Task(VOLTAGE_READING_INTERVAL, TASK_FOREVER, std::bind(&KlineCarduinoNode::voltageCallback, this), this->scheduler, true);

	this->afterReadExecutors = new AfterReadExecutors();
	this->afterReadExecutors->addExecutor(new FuelConsumptionExecutor());

	this->klineConnected = false;
};

void KlineCarduinoNode::readValues() {
	// bool isOtaMode = this->getSettingValue(&Setting::OTA_MODE)->value->boolValue;
	// Serial.println("KlineCarduinoNode::readValues() start");
	// unsigned long start = millis();
	// Serial.println("start readValues()");
	if(this->getSettingValue(&Setting::OTA_MODE)->value->boolValue) {
		if(klineConnected) {
			kLine->disconnect();
			this->klineConnected = false;
			this->lastConnectedEcu = nullptr;
		}
	} else { // i can connect & read
		// Serial.print("KlineCarduinoNode::readValues() this->otaMode ");
		// Serial.println(this->otaMode ? "true" : "false");
		if(!this->isEnabled) {
			return; // don't read values if node is not enabled
		}
		uint8_t ecusToReadSize = ValueToReadEnum::getEcusToReadSize();
		KlineEcuEnum **ecusToRead = ValueToReadEnum::getEcusToRead();

		for(uint8_t ecuIndex = 0; ecuIndex < ecusToReadSize; ecuIndex++) { // iterate over all ECUs used in ValueToReadEnum.h
			KlineEcuEnum *klineEcuEnum = ecusToRead[ecuIndex];
			uint8_t blockValuesByEcuSize = ValueToReadEnum::getBlockValuesByEcuSize(*klineEcuEnum);
			if(blockValuesByEcuSize > 0) {
				uint8_t* blockValuesByEcu = ValueToReadEnum::getBlockValuesByEcu(*klineEcuEnum);

				if(lastConnectedEcu == nullptr || (lastConnectedEcu != nullptr && lastConnectedEcu->id != klineEcuEnum->id)) {
					// Serial.println("KlineCarduinoNode::readValues() trying to connect ");
					#ifdef DEBUG_KLINE_NODE
					printlnWrapper("KlineCarduinoNode: Attempting connection to ECU " + String(klineEcuEnum->address, HEX));
					#endif
					this->klineConnected = kLine->attemptConnect(klineEcuEnum->address, klineEcuEnum->baud) == KLineKWP1281Lib::SUCCESS; // connect here to avoid connection to ecus that won't read any value
					// Serial.print("KlineCarduinoNode::readValues() this->klineConnected ");
					// Serial.println(this->klineConnected ? "true" : "false");
					// if(this->klineConnected) {
					// 	lastConnectedEcu = klineEcuEnum;
					// }
					#ifdef DEBUG_KLINE_NODE
					printlnWrapper("KlineCarduinoNode: Connection " + String(this->klineConnected ? "SUCCESS" : "FAILED"));
					#endif
				}
				if(this->klineConnected) {
					// if(this->getSettingValue(&Setting::OTA_MODE)->value->boolValue) {
					// 	this->otaShutdown();
					// }

					lastConnectedEcu = klineEcuEnum;
					for(uint8_t blockValuesByEcuIndex = 0; blockValuesByEcuIndex < blockValuesByEcuSize; blockValuesByEcuIndex++) {
						uint8_t block = blockValuesByEcu[blockValuesByEcuIndex];
						uint8_t valuesByEcuBlockSize = ValueToReadEnum::getValuesByEcuBlockSize(*klineEcuEnum, block);
						ValueToReadEnum** valuesByEcuBlock = ValueToReadEnum::getValuesByEcuBlock(*klineEcuEnum, block);

						for(uint8_t valuesByEcuBlockIndex = 0; valuesByEcuBlockIndex < valuesByEcuBlockSize; valuesByEcuBlockIndex++) {
							ValueToReadEnum *valueToReadEnum = valuesByEcuBlock[valuesByEcuBlockIndex];

							uint8_t measurements[3 * 4]; //buffer to store the measurements; each measurement takes 3 bytes; one block contains 4 measurements
							uint8_t amount_of_measurements = 0;
							switch (kLine->readGroup(amount_of_measurements, block, measurements, sizeof(measurements))) {
								case KLineKWP1281Lib::ERROR:
									Serial.println("Error reading measurements!");
									break;
								
								case KLineKWP1281Lib::FAIL:
									Serial.print("Block ");
									Serial.print(valueToReadEnum->group);
									Serial.println(" does not exist!");
									break;
								
								case KLineKWP1281Lib::SUCCESS:
									/*
										The getMeasurementType() function can return:
										*KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
										*KLineKWP1281Lib::UNITS   - the measurement contains human-readable text in the units string
										*KLineKWP1281Lib::VALUE   - "regular" measurement, with a value and units
									*/
									switch(KLineKWP1281Lib::getMeasurementType(valueToReadEnum->groupIndex, amount_of_measurements, measurements, sizeof(measurements))) {
										//Value and units
										case KLineKWP1281Lib::VALUE: {
											float value = KLineKWP1281Lib::getMeasurementValue(valueToReadEnum->groupIndex, amount_of_measurements, measurements, sizeof(measurements));
											
											// Serial.print("KlineCarduinoNode::readValues() read value ");
											// Serial.print(" ");
											// Serial.print(valueToReadEnum->name);
											// Serial.print(" ");
											// Serial.println(value);
											#ifdef DEBUG_KLINE_NODE
											printlnWrapper("KlineCarduinoNode: Read " + String(valueToReadEnum->name) + " = " + String(value));
											#endif

											if(valueToReadEnum->send) {
												CarstatusMessage *c = nullptr;
												if(valueToReadEnum->carstatus.type->id == CanbusMessageType::INT.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, int(value));
												} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::FLOAT.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, value);
												} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::BOOL.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, value == 1);
												}
												#ifdef DEBUG_KLINE_NODE
												printlnWrapper("KlineCarduinoNode: Sending CAN Message ID " + String(valueToReadEnum->carstatus.id));
												#endif
												sendCanbusMessage(c);
												delete c;
											}

											if(valueToReadEnum->carstatus.type->id == CanbusMessageType::INT.id) {
												valueToReadEnum->lastReadValue.intValue = int(value);
											} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::FLOAT.id) {
												valueToReadEnum->lastReadValue.floatValue = value;
											} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::BOOL.id) {
												valueToReadEnum->lastReadValue.boolValue = value == 1;
											}

											break;
										}
										
										//Units string containing text
										case KLineKWP1281Lib::UNITS: {
											//Will hold the measurement's units
											char units_string[16];
											Serial.println(KLineKWP1281Lib::getMeasurementUnits(valueToReadEnum->groupIndex, amount_of_measurements, measurements, sizeof(measurements), units_string, sizeof(units_string)));
											break;
										}
										
										//Invalid measurement index
										case KLineKWP1281Lib::UNKNOWN: {
											Serial.println("N/A");
											break;
										}
									}
									break;
							}
						}
						delete[] valuesByEcuBlock;
					}
					this->afterReadExecutors->execute(this);
				} else {
					printlnWrapper("KlineCarduinoNode::readValues could not connect to ECU");
				}
				delete[] blockValuesByEcu;
			}
		}
		
		delete ecusToRead;
	}
	// Serial.print("end readValues() ");
	// Serial.println((millis() - start));
	// Serial.println("KlineCarduinoNode::readValues() finish");
};

void KlineCarduinoNode::loop () {
	CarduinoNode::loop();
}

void KlineCarduinoNode::voltageCallback() {
	int sensorValue = analogRead(VOLTAGE_READING_PIN); // Legge il valore del pin analogico A0
	float voltageOut = sensorValue * (3.3 / 1023.0); // Convertilo in una tensione tra 0 e 3.3V
	float voltageIn = voltageOut * (R1 + R2) / R2; // Calcola la tensione in ingresso utilizzando il partitore di tensione

	printlnWrapper("KlineCarduinoNode::voltageCallback() A0: " + String(sensorValue) + ", voltage 0-3.3V: " + String(voltageOut) + ", input voltage: " + String(voltageIn));

	CarstatusMessage m(&Carstatus::BATTERY_VOLTAGE, voltageIn);
    this->sendCanbusMessage(&m);
}
