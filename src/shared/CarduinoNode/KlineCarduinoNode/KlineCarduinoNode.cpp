#include "KlineCarduinoNode.h"

KlineCarduinoNode::KlineCarduinoNode(uint8_t id, uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, true, true) {
    this->pin_rx = pin_rx;
    this->pin_tx = pin_tx;
	this->softwareSerial = new SoftwareSerial(pin_rx, pin_tx);

	std::function<void(unsigned long)> beginFunction = [&](unsigned long baud){
		softwareSerial->begin(baud);
	};
	std::function<void()> endFunction = [&](){
		softwareSerial->end();
	};
	std::function<void(uint8_t)> sendFunction = [&](uint8_t data){
		softwareSerial->write(data);
	};
	std::function<bool(uint8_t&)> receiveFunction = [&](uint8_t &data){
		if (softwareSerial->available()) {
			data = softwareSerial->read();
			return true;
		}
		return false;
	};
	
    this->kLine = new KLineKWP1281Lib(beginFunction, endFunction, sendFunction, receiveFunction, pin_tx, true);
	this->lastConnectedEcu = nullptr;

	this->scheduler = new Scheduler();

    KlineCallback<void(void)>::func = std::bind(&KlineCarduinoNode::readValues, this);
    readValuesTask = new Task(500, TASK_FOREVER, static_cast<TaskCallback>(KlineCallback<void(void)>::callback), scheduler, true);

	this->afterReadExecutors = new AfterReadExecutors();
	this->afterReadExecutors->addExecutor(new FuelConsumptionExecutor());

	this->klineConnected = false;
};

void KlineCarduinoNode::readValues() {
	// Serial.println("KlineCarduinoNode::readValues() start");
	// unsigned long start = millis();
	// Serial.println("start readValues()");
	// if(!this->otaMode) {
		// Serial.print("KlineCarduinoNode::readValues() this->otaMode ");
		// Serial.println(this->otaMode ? "true" : "false");
		uint8_t ecusToReadSize = ValueToReadEnum::getEcusToReadSize();
		KlineEcuEnum **ecusToRead = ValueToReadEnum::getEcusToRead();

		for(uint8_t ecuIndex = 0; ecuIndex < ecusToReadSize; ecuIndex++) { // iterate over all ECUs used in ValueToReadEnum.h
			KlineEcuEnum *klineEcuEnum = ecusToRead[ecuIndex];
			uint8_t blockValuesByEcuSize = ValueToReadEnum::getBlockValuesByEcuSize(*klineEcuEnum);
			if(blockValuesByEcuSize > 0) {
				uint8_t* blockValuesByEcu = ValueToReadEnum::getBlockValuesByEcu(*klineEcuEnum);

				if(lastConnectedEcu == nullptr || (lastConnectedEcu != nullptr && lastConnectedEcu->id != klineEcuEnum->id)) {
					// Serial.println("KlineCarduinoNode::readValues() trying to connect ");
					this->klineConnected = kLine->attemptConnect(klineEcuEnum->address, klineEcuEnum->baud) == KLineKWP1281Lib::SUCCESS; // connect here to avoid connection to ecus that won't read any value
					// Serial.print("KlineCarduinoNode::readValues() this->klineConnected ");
					// Serial.println(this->klineConnected ? "true" : "false");
					if(this->klineConnected) {
						lastConnectedEcu = klineEcuEnum;
					}
				}
				if(this->klineConnected) {
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
											
											Serial.print("KlineCarduinoNode::readValues() read value ");
											Serial.print(" ");
											Serial.print(valueToReadEnum->name);
											Serial.print(" ");
											Serial.println(value);

											if(valueToReadEnum->send) {
												CarstatusMessage *c = nullptr;
												if(valueToReadEnum->carstatus.type->id == CanbusMessageType::INT.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, int(value));
												} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::FLOAT.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, value);
												} else if(valueToReadEnum->carstatus.type->id == CanbusMessageType::BOOL.id) {
													c = new CarstatusMessage(&valueToReadEnum->carstatus, value == 1);
												}
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
					delete[] blockValuesByEcu;

					
					// Serial.println("KlineCarduinoNode::readValues() trying to call  this->afterReadExecutors->execute(this)");
					this->afterReadExecutors->execute(this);
					// Serial.println("KlineCarduinoNode::readValues() called  this->afterReadExecutors->execute(this)");
				} else {
					printlnWrapper("KlineCarduinoNode::readValues could not connect to ECU");
				}
			}
		}
		
		delete ecusToRead;
	// }
	// Serial.print("end readValues() ");
	// Serial.println((millis() - start));
	// Serial.println("KlineCarduinoNode::readValues() finish");
};

void KlineCarduinoNode::loop () {
	CarduinoNode::loop();
	this->scheduler->execute();
}
