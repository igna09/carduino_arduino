#include "CommunicationCarduinoNode.h"

CommunicationCarduinoNode::CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, true, true) {
    this->addSetting(&Setting::BLE_PAIRING, false, [&](SettingInformation *settingInformation){
        if(settingInformation->value->boolValue) {
            this->enableNewPairing();
        } else {
            this->disableNewPairing();
        }
    });
    this->restoreSettings();

    this->authenticatedBdAddress = nullptr;

    // Start BLE service
    /*BLEDevice::init("ESP32");
    Serial.println("BLE started!");

    bleClient = new BLEClient();*/
    BLEDevice::init("ESP32");
    GAPCallback<void(esp_gap_ble_cb_event_t, esp_ble_gap_cb_param_t*)>::func = std::bind(&CommunicationCarduinoNode::customGapCallback, this, std::placeholders::_1, std::placeholders::_2);
    BLEDevice::setCustomGapHandler(static_cast<gap_event_handler>(GAPCallback<void(esp_gap_ble_cb_event_t, esp_ble_gap_cb_param_t*)>::callback));
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new MyBLESecurityCallbacks(this));

    // Create the BLE Server
    bleServer = BLEDevice::createServer();
    bleServer->setCallbacks(new MyBLEServerCallbacks(this));

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    // pAdvertising->addServiceUUID(SERVICE_UUID);
    bleServer->getAdvertising()->start();

    pSecurity = new BLESecurity();
    pSecurity->setStaticPIN(123456);
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
    // pSecurity->setKeySize(16); //the key size should be 7~16 bytes
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    printlnWrapper("Waiting a client connection to notify...");

    // pBLEScan = BLEDevice::getScan(); //create new scan
    // pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    // pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    // pBLEScan->setInterval(100);
    // pBLEScan->setWindow(99);  // less or equal setInterval value

    // new Task(5500, TASK_FOREVER, [&](){
    //     BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    //     Serial.print("Devices found: ");
    //     Serial.println(foundDevices.getCount());
    //     Serial.println("Scan done!");
    //     for(uint8_t i = 0; i < foundDevices.getCount(); i++) {
    //         BLEAdvertisedDevice bleAdvertisedDevice = foundDevices.getDevice(i);
    //         Serial.println(bleAdvertisedDevice.toString().c_str());
    //     }
    //     pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
    // }, this->scheduler, true);

    this->rssiTask = new Task(1000, TASK_FOREVER, [&](){
        esp_err_t rc = esp_ble_gap_read_rssi(*this->authenticatedBdAddress->getNative());
    }, this->scheduler, false);

    this->disableNewPairing();

    usbExecutor->addExecutor(new CommunicationCarduinoNodeEvents());

    /**
     * TMP
     */
    otaStartup();
    this->enable();

    logToFile("setup done");
};

void CommunicationCarduinoNode::loop() {
    CarduinoNode::loop();

    // Increment the counter from 0 to 9
    // pCharacteristic->setValue(String(counter).c_str());
    // pCharacteristic->notify();

    // Serial.print("Sending value: ");
    // Serial.println(counter);

    // delay(1000); // Adjust the delay based on your application requirements

    // Reset counter when it reaches 9
    // if (counter == 9) {
    //     counter = 0;
    // } else {
    //     counter++;
    // }

    // Scan for BLE devices
    /*BLEDevice* pBLEDevice = BLE.scan();
    if (pBLEDevice) {
        // Check if the device name matches the target device
        if (strcmp(pBLEDevice->getName(), bleDeviceName) == 0) {
        // Connect to the BLE device
        bleClient->connect(pBLEDevice);
        if (bleClient->isConnected()) {
            Serial.println("Connected to BLE device!");

            // Read the RSSI value
            int rssi = bleClient->getRssi();
            Serial.print("RSSI: ");
            Serial.println(rssi);

            // Update LED based on RSSI value (example)
            if (rssi >= -60) {
            // digitalWrite(ledPin, HIGH); // Strong signal, turn on LED
            } else {
            // digitalWrite(ledPin, LOW); // Weak signal, turn off LED
            }

            // Disconnect from the BLE device
            bleClient->disconnect();
            Serial.println("Disconnected from BLE device!");
        } else {
            Serial.println("Failed to connect to BLE device!");
        }
        }
    } else {
        Serial.println("No BLE devices found!");
    }*/
}

void CommunicationCarduinoNode::clientAuthenticated() {
    printlnWrapper("CommunicationCarduinoNode::clientAuthenticated");

    // uint8_t connectedCount = bleServer->getConnectedCount();
    // std::map<uint16_t, conn_status_t> peerDevices = bleDevice->getPeerDevices(false);
    // std::size_t peerDevicesCount = peerDevices.size();
    // Serial.println(peerDevicesCount);
    printlnWrapper("----------------");

    std::map<uint16_t, conn_status_t>::iterator it;
    for(it = bleDevice->getPeerDevices(false).begin(); it != bleDevice->getPeerDevices(false).end(); ++it){
        conn_status_t connStatus = it->second;
        BLEClient* bleClient = (BLEClient*) connStatus.peer_device;

        // printlnWrapper(bleDevice->getRssi());
        // printlnWrapper(bleClient->getPeerAddress().toString().c_str());
        // printlnWrapper(bleClient->toString().c_str());
        // Serial.println(bleClient->getConnId());
        printlnWrapper("----------------");
    }

    // for(auto &myPair : BLEDevice::getPeerDevices(true)) {
	// 	conn_status_t connStatus = (conn_status_t)myPair.second;
    //     BLEClient* bleDevice = (BLEClient*) connStatus.peer_device;

    //     printlnWrapper(bleDevice->getRssi());
    //     printlnWrapper(bleDevice->getPeerAddress().toString().c_str());
    //     printlnWrapper(bleDevice->toString().c_str());
    //     printlnWrapper("----------------");
	// 	// if(((BLEClient*)conn_status.peer_device)->getGattcIf() == gattc_if || ((BLEClient*)conn_status.peer_device)->getGattcIf() == ESP_GATT_IF_NONE || gattc_if == ESP_GATT_IF_NONE){
	// 	// 	((BLEClient*)conn_status.peer_device)->gattClientEventHandler(event, gattc_if, param);
	// 	// }
	// }

    // std::map<uint16_t, conn_status_t>::iterator it;
    // for(it = bleServer->getPeerDevices(false).begin(); it != bleServer->getPeerDevices(false).end(); ++it){
    //     conn_status_t connStatus = it->second;
    //     BLEClient* bleDevice = (BLEClient*) connStatus.peer_device;

    //     printlnWrapper(bleDevice->getRssi());
    //     printlnWrapper(bleDevice->getPeerAddress().toString().c_str());
    //     printlnWrapper(bleDevice->toString().c_str());
    //     printlnWrapper("----------------");
    // }
}

void CommunicationCarduinoNode::clearWhitelist() {
    printlnWrapper("CommunicationCarduinoNode::clearWhitelist");
    esp_ble_gap_clear_whitelist();
}

void CommunicationCarduinoNode::customGapCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    // Serial.println("customGapHandler");
    // Serial.println(BLEUtils::gapEventToString(event));
    switch(event) {
        case ESP_GAP_BLE_AUTH_CMPL_EVT: {
            String message = "[bd_addr: ";
            message += BLEAddress(param->ble_security.auth_cmpl.bd_addr).toString().c_str();
            message += ", success: ";
            message += param->ble_security.auth_cmpl.success;
            message += ", fail_reason: ";
            message += param->ble_security.auth_cmpl.fail_reason;
            message += ", dev_type: ";
            message += BLEUtils::devTypeToString(param->ble_security.auth_cmpl.dev_type);
            message += "]";
            // log_e("[bd_addr: %s, key_present: %d, key: ***, key_type: %d, success: %d, fail_reason: %d, addr_type: ***, dev_type: %s]",
            //     BLEAddress(param->ble_security.auth_cmpl.bd_addr).toString().c_str(),
            //     param->ble_security.auth_cmpl.key_present,
            //     param->ble_security.auth_cmpl.key_type,
            //     param->ble_security.auth_cmpl.success,
            //     param->ble_security.auth_cmpl.fail_reason,
            //     BLEUtils::devTypeToString(param->ble_security.auth_cmpl.dev_type)
            // );
            printlnWrapper(message);
            logToFile(message);
            if(param->ble_security.auth_cmpl.success) {
                this->authenticatedBdAddress = new BLEAddress(param->ble_security.auth_cmpl.bd_addr);
                this->rssiTask->enable();

                // BLEDevice::whiteListAdd(*this->authenticatedBdAddress); //https://github.com/espressif/arduino-esp32/issues/9404
            } else {
                // BLEDevice::blackListAdd(*this->authenticatedBdAddress);
            }
            break;
        } // ESP_GAP_BLE_AUTH_CMPL_EVT
        case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT: {
            String message = "[status: ";
            message += param->read_rssi_cmpl.status;
            message += ", rssi: ";
            message += param->read_rssi_cmpl.rssi;
            message += ", remote_addr: ";
            message += BLEAddress(param->read_rssi_cmpl.remote_addr).toString().c_str();
            message += "]";
            // log_e("[status: %d, rssi: %d, remote_addr: %s]",
            //         param->read_rssi_cmpl.status,
            //         param->read_rssi_cmpl.rssi,
            //         BLEAddress(param->read_rssi_cmpl.remote_addr).toString().c_str()
            // );
            printlnWrapper(message);
            logToFile(message);

            if(param->read_rssi_cmpl.rssi > -60) {
                sendEvent(&Event::UNLOCK_CAR);
            } else {
                sendEvent(&Event::LOCK_CAR);
            }

            /**
             * start here a task that check phone rssi
             */

            break;
        } // ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT
        case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT: {
            
        } // ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT
    }
}

void CommunicationCarduinoNode::logToFile(String message) {
    tm timeInfo;
    getLocalTime(&timeInfo);
    String localTime = String(timeInfo.tm_year) + "-" + String(timeInfo.tm_mon) + "-" + String(timeInfo.tm_yday) + " " + String(timeInfo.tm_hour) + ":" + String(timeInfo.tm_min) + ":" + String(timeInfo.tm_sec);
    message = localTime + " " + message;
    appendToFile("/node_logs.txt", message);
}

void CommunicationCarduinoNode::enableNewPairing() {
    // bleServer->getAdvertising()->setScanFilter(false,false);
    // pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND); // https://github.com/nkolban/esp32-snippets/issues/613#issuecomment-496400715
    bleServer->getAdvertising()->start();
    disabledPairing = false;
}

void CommunicationCarduinoNode::disableNewPairing() {
    // bleServer->getAdvertising()->setScanFilter(false,true);
    // pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY); // https://github.com/nkolban/esp32-snippets/issues/613#issuecomment-496400715
    bleServer->getAdvertising()->stop();
    disabledPairing = true;
}

void CommunicationCarduinoNode::test() {
    clearWhitelist();
}
