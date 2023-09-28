#include "KlineCarduinoNode.h"

KlineCarduinoNode::KlineCarduinoNode(uint8_t pin_rx, uint8_t pin_tx, int cs, int interruptPin, char *ssid,  char *password) : CarduinoNode(cs, interruptPin, ssid,  password) {
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
};

void KlineCarduinoNode::loop() {
	CarduinoNode::loop();

	for(uint8_t ecuIndex = 0; ecuIndex < KlineEcuEnum::getSize(); ecuIndex++) { // iterate over all ECUs declared in KlineEcuEnum.h
		KlineEcuEnum *klineEcuEnum = (KlineEcuEnum*) KlineEcuEnum::getValues()[ecuIndex];
		const ValueToReadEnum** valuesToReadEnumByEcu = ValueToReadEnum::getValuesByEcu(*klineEcuEnum);
		
		for(uint8_t valueToReadEnumIndex = 0; valueToReadEnumIndex < ValueToReadEnum::getSizeByEcu(*klineEcuEnum); valueToReadEnumIndex++) { // iterate over all ecu values
			kLine->connect(klineEcuEnum->address, klineEcuEnum->address); // connect here to avoid connection to ecus that won't read any value

			ValueToReadEnum valueToReadEnum = *valuesToReadEnumByEcu[valueToReadEnumIndex];

			uint8_t measurements[3 * 4]; //buffer to store the measurements; each measurement takes 3 bytes; one block contains 4 measurements
			uint8_t amount_of_measurements = 0;
			switch (kLine->readGroup(amount_of_measurements, valueToReadEnum.group, measurements, sizeof(measurements))) {
				case KLineKWP1281Lib::ERROR:
					Serial.println("Error reading measurements!");
					break;
				
				case KLineKWP1281Lib::FAIL:
					Serial.print("Block ");
					Serial.print(valueToReadEnum.group);
					Serial.println(" does not exist!");
					break;
				
				case KLineKWP1281Lib::SUCCESS:
					/*
						The getMeasurementType() function can return:
						*KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
						*KLineKWP1281Lib::UNITS   - the measurement contains human-readable text in the units string
						*KLineKWP1281Lib::VALUE   - "regular" measurement, with a value and units
					*/
					switch(KLineKWP1281Lib::getMeasurementType(valueToReadEnum.groupIndex, amount_of_measurements, measurements, sizeof(measurements))) {
						//Value and units
						case KLineKWP1281Lib::VALUE: {
							float value = KLineKWP1281Lib::getMeasurementValue(valueToReadEnum.groupIndex, amount_of_measurements, measurements, sizeof(measurements));

							if(valueToReadEnum.carstatus.type.id == CanbusMessageType::INT.id) {
								sendCanbusMessage(CarstatusMessage(&valueToReadEnum.carstatus, int(value)));
							} else if(valueToReadEnum.carstatus.type.id == CanbusMessageType::FLOAT.id) {
								sendCanbusMessage(CarstatusMessage(&valueToReadEnum.carstatus, value));
							} else if(valueToReadEnum.carstatus.type.id == CanbusMessageType::BOOL.id) {
								sendCanbusMessage(CarstatusMessage(&valueToReadEnum.carstatus, value == 1));
							}
							break;
						}
						
						//Units string containing text
						case KLineKWP1281Lib::UNITS: {
							//Will hold the measurement's units
							char units_string[16];
							Serial.println(KLineKWP1281Lib::getMeasurementUnits(valueToReadEnum.groupIndex, amount_of_measurements, measurements, sizeof(measurements), units_string, sizeof(units_string)));
							break;
						}
						
						//Invalid measurement index
						case KLineKWP1281Lib::UNKNOWN: {
							Serial.println("N/A");
							break;
						}
					}
				}
				break;
		}
	}
};
