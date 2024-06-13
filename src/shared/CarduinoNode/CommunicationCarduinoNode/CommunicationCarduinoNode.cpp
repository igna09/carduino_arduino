#include "CommunicationCarduinoNode.h"

CommunicationCarduinoNode::CommunicationCarduinoNode(uint8_t id, int cs, int interruptPin, const char *ssid, const char *password) : CarduinoNode(id, cs, interruptPin, ssid,  password, false, true, true) {
    this->restoreSettings();

    // counter = 0;

    /*BLEDevice::init("ESP32");

    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);

    BLEDevice::setSecurityCallbacks(new MySecurity());

    pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic->setValue("Hello World");
    pCharacteristic->setCallbacks(new MyCallbacks());
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    BLESecurity *pSecurity = new BLESecurity();
    // uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint32_t passkey = 123456;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t iocap = ESP_IO_CAP_NONE; //set the IO capability to No Input No Output
    uint8_t auth_req = ESP_LE_AUTH_BOND; //bonding with peer device after authentication
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    pSecurity->setAuthenticationMode(auth_req);
    pSecurity->setCapability(iocap);
    pSecurity->setKeySize(key_size);
    pSecurity->setInitEncryptionKey(init_key);
    pSecurity->setRespEncryptionKey(rsp_key);
    delete pSecurity;
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    // esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
    Serial.println("Characteristic defined! Now you can read it on your phone!");*/

    // Start BLE service
    /*BLEDevice::init("ESP32");
    Serial.println("BLE started!");

    bleClient = new BLEClient();*/
    BLEDevice::init("ESP32");

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    // pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");

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
