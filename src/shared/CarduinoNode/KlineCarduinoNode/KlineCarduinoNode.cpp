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

	ecusToRead = EcusToRead();
	
	EcuToRead instrumentEcu = EcuToRead();
	instrumentEcu.ecu = KlineEcuEnum::INSTRUMENT;
	BlockToRead instrumentBlock1 = BlockToRead();
	instrumentBlock1.block = 1;
	instrumentBlock1.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_1, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read instrument ecu block 1 value 1");
	}));
	instrumentBlock1.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_3, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read instrument ecu block 1 value 3");
	}));
	instrumentEcu.addBlockToRead(instrumentBlock1);
	ecusToRead.addEcuToRead(instrumentEcu);

	EcuToRead engineEcu = EcuToRead();
	engineEcu.ecu = KlineEcuEnum::ENGINE;
	BlockToRead engineBlock1 = BlockToRead();
	engineBlock1.block = 1;
	engineBlock1.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_1, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read engine ecu block 1 value 1");
	}));
	engineBlock1.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_2, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read engine ecu block 1 value 2");
	}));
	engineEcu.addBlockToRead(engineBlock1);
	BlockToRead engineBlock2 = BlockToRead();
	engineBlock2.block = 2;
	engineBlock2.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_2, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read engine ecu block 2 value 2");
	}));
	engineBlock2.addValueToRead(ValueToRead(ValueToReadEnum::VALUE_TO_READ_4, [&](float v){
		//send value with category id car status and message relative to block/index
		Serial.print("Read engine ecu block 2 value 4");
	}));
	engineEcu.addBlockToRead(engineBlock2);
	ecusToRead.addEcuToRead(engineEcu);
};

void KlineCarduinoNode::loop() {
	CarduinoNode::loop();

	for(uint8_t ecuIndex = 0; ecuIndex < ecusToRead.size; ecuIndex++) {
		EcuToRead ecuToRead = ecusToRead.ecusToRead[ecuIndex];

		kLine->connect(ecuToRead.ecu.address, ecuToRead.ecu.baud);

		for(uint8_t blockIndex = 0; blockIndex < ecuToRead.size; blockIndex++) {
			BlockToRead blockToRead = ecuToRead.blocksToRead[blockIndex];

			for(uint8_t valueIndex = 0; valueIndex < blockToRead.size; valueIndex++) {
				ValueToRead valueToRead = blockToRead.valuesToRead[valueIndex];

				uint8_t measurements[3 * 4]; //buffer to store the measurements; each measurement takes 3 bytes; one block contains 4 measurements
				uint8_t amount_of_measurements = 0;
				switch (kLine->readGroup(amount_of_measurements, blockToRead.block, measurements, sizeof(measurements))) {
					case KLineKWP1281Lib::ERROR:
						Serial.println("Error reading measurements!");
						break;
					
					case KLineKWP1281Lib::FAIL:
						Serial.print("Block ");
						Serial.print(blockToRead.block);
						Serial.println(" does not exist!");
						break;
					
					case KLineKWP1281Lib::SUCCESS:
						//Will hold the measurement's units
						char units_string[16];
						
						//Display the selected measurement.
						
						/*
							The getMeasurementType() function can return:
							*KLineKWP1281Lib::UNKNOWN - index out of range (measurement doesn't exist in block)
							*KLineKWP1281Lib::UNITS   - the measurement contains human-readable text in the units string
							*KLineKWP1281Lib::VALUE   - "regular" measurement, with a value and units
						*/
						switch (KLineKWP1281Lib::getMeasurementType(valueToRead.valueToReadEnum.value, amount_of_measurements, measurements, sizeof(measurements))) {
							//Value and units
							case KLineKWP1281Lib::VALUE:
								valueToRead.floatCallback(KLineKWP1281Lib::getMeasurementValue(valueToRead.valueToReadEnum.value, amount_of_measurements, measurements, sizeof(measurements)));
								break;
							
							//Units string containing text
							case KLineKWP1281Lib::UNITS:
								valueToRead.charCallback(KLineKWP1281Lib::getMeasurementUnits(valueToRead.valueToReadEnum.value, amount_of_measurements, measurements, sizeof(measurements), units_string, sizeof(units_string)));
								break;
							
							//Invalid measurement index
							case KLineKWP1281Lib::UNKNOWN:
								Serial.println("N/A");
								break;
						}
						break;
				}
			}
		}
	}
};
