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

    /**
     * NimBLE
     */
    NimBLEDevice::init("ESP32");
    restoreWhitelist();
    
    bleServer = NimBLEDevice::createServer();
    bleServer->setCallbacks(new MyBLEServerCallbacks(this));

    NimBLEDevice::setSecurityAuth(true, false, true);
    NimBLEDevice::setSecurityIOCap(0); //ESP_IO_CAP_OUT
    NimBLEDevice::setSecurityInitKey(1 << 0 | 1 << 1); //ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK
    NimBLEDevice::setSecurityRespKey(1 << 0 | 1 << 1); //ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK
    // pSecurity->setKeySize(16); //the key size should be 7~16 bytes

    GAPCallback<int(ble_gap_event *event, void *arg)>::func = std::bind(&CommunicationCarduinoNode::customGapCallback, this, std::placeholders::_1, std::placeholders::_2);
    NimBLEDevice::setCustomGapHandler(static_cast<gap_event_handler>(GAPCallback<int(ble_gap_event *event, void *arg)>::callback));
    
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->start(); 

    printlnWrapper("Waiting a client connection to notify...");

    this->rssiTask = new Task(1000, TASK_FOREVER, [&](){
        // esp_err_t rc = esp_ble_gap_read_rssi((uint8_t*)this->authenticatedBdAddress->getNative());
        // ble_gap_conn_rssi(this->authenticatedBdAddress->, int8_t *out_rssi);
    }, this->scheduler, true);

    // this->disableNewPairing();

    // usbExecutor->addExecutor(new CommunicationCarduinoNodeEvents());

    /**
     * TMP
     */
    otaStartup();
    // this->enable();

    printlnWrapper("setup done");
};

void CommunicationCarduinoNode::loop() {
    CarduinoNode::loop();
}

void CommunicationCarduinoNode::clientAuthenticated(NimBLEConnInfo info) {
    printlnWrapper("CommunicationCarduinoNode::clientAuthenticated");

    if(!info.getIdAddress().isRpa() && info.isBonded()){
        if(!NimBLEDevice::onWhiteList(info.getIdAddress())) {
            bleServer->disconnect(info.getConnHandle());
            NimBLEDevice::deleteBond(info.getIdAddress());
            printlnWrapper("CommunicationCarduinoNode::clientAuthenticated not on whitelist, cannot connect");
        } else {
            String message = "authenticated [bd_addr: ";
            message += info.getAddress().toString().c_str();
            message += ", success: ";
            message += info.isAuthenticated();
            message += ", bonded: ";
            message += info.isBonded();
            message += ", encrypted: ";
            message += info.isEncrypted();
            message += "]";
            printlnWrapper(message, true);

            //manage rssi
            int8_t rssi;
            ble_gap_conn_rssi(info.getConnHandle(), &rssi);
            Serial.println("RSSI " + String(rssi));
        }
    }
}

void CommunicationCarduinoNode::clearWhitelist() {
    printlnWrapper("CommunicationCarduinoNode::clearWhitelist");
    for(uint8_t i = 0; i < NimBLEDevice::getWhiteListCount(); i++) {
        auto address = NimBLEDevice::getWhiteListAddress(i);
        NimBLEDevice::whiteListRemove(address);
    }
    backupWhitelist();
}

void CommunicationCarduinoNode::listWhitelist() {
    printlnWrapper("CommunicationCarduinoNode::listWhitelist");
    for(uint8_t i = 0; i < NimBLEDevice::getWhiteListCount(); i++) {
        auto address = NimBLEDevice::getWhiteListAddress(i);
        printlnWrapper(String(address.toString().c_str()));
    }
}

void CommunicationCarduinoNode::backupWhitelist() {
    printlnWrapper("CommunicationCarduinoNode::backupWhitelist");
    File whitelistFile = getOrCreateFile("/whitelist.json", "w");

    JsonDocument whitelistJson;
    JsonArray settingsJsonArray = whitelistJson.to<JsonArray>();

    for(uint8_t i = 0; i < NimBLEDevice::getWhiteListCount(); i++) {
        auto address = NimBLEDevice::getWhiteListAddress(i);
        printlnWrapper("CommunicationCarduinoNode::backupWhitelist backup whitelist address " + String(address.toString().c_str()));
        settingsJsonArray.add(address.toString().c_str());
    }

    serializeJson(whitelistJson, whitelistFile);

    whitelistFile.close();
}

void CommunicationCarduinoNode::restoreWhitelist() {
    printlnWrapper("CommunicationCarduinoNode::restoreWhitelist");

    JsonDocument whitelistJson;
    File whitelistFile = getOrCreateFile("/whitelist.json", "r");
    deserializeJson(whitelistJson, whitelistFile);
    whitelistFile.close();

    for(JsonVariant item : whitelistJson.as<JsonArray>()) {
        String mac = item.as<String>();
        printlnWrapper("CommunicationCarduinoNode::restoreWhitelist restore whitelist address " + mac);
        NimBLEDevice::whiteListAdd(NimBLEAddress(mac.c_str(), BLE_ADDR_PUBLIC));
    }
}

int CommunicationCarduinoNode::customGapCallback(ble_gap_event *event, void *arg) {
    printlnWrapper("CommunicationCarduinoNode::customGapCallback " +  String(NimBLEUtils::gapEventToString(event->type)));
    // switch(event->type) {
    //     case ESP_GAP_BLE_AUTH_CMPL_EVT: {
    //         if(param->ble_security.auth_cmpl.success) {
    //             this->authenticatedBdAddress = new BLEAddress(param->ble_security.auth_cmpl.bd_addr);
    //             this->rssiTask->enable();

    //             // BLEDevice::whiteListAdd(*this->authenticatedBdAddress); //https://github.com/espressif/arduino-esp32/issues/9404
    //         } else {
    //             // BLEDevice::blackListAdd(*this->authenticatedBdAddress);
    //         }
    //         break;
    //     } // ESP_GAP_BLE_AUTH_CMPL_EVT
    //     case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT: {
    //         String message = "[status: ";
    //         message += param->read_rssi_cmpl.status;
    //         message += ", rssi: ";
    //         message += param->read_rssi_cmpl.rssi;
    //         message += ", remote_addr: ";
    //         message += BLEAddress(param->read_rssi_cmpl.remote_addr).toString().c_str();
    //         message += "]";
    //         // log_e("[status: %d, rssi: %d, remote_addr: %s]",
    //         //         param->read_rssi_cmpl.status,
    //         //         param->read_rssi_cmpl.rssi,
    //         //         BLEAddress(param->read_rssi_cmpl.remote_addr).toString().c_str()
    //         // );
    //         printlnWrapper(message);
    //         logToFile(message);

    //         if(param->read_rssi_cmpl.rssi > -60) {
    //             sendEvent(&Event::UNLOCK_CAR);
    //         } else {
    //             sendEvent(&Event::LOCK_CAR);
    //         }

    //         /**
    //          * start here a task that check phone rssi
    //          */

    //         break;
    //     } // ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT
    //     case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT: {
            
    //     } // ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT
    // }
    return 0;
}

void CommunicationCarduinoNode::enableNewPairing() {
    // bleServer->getAdvertising()->setScanFilter(false,false);
    disabledPairing = false;
}

void CommunicationCarduinoNode::disableNewPairing() {
    // bleServer->getAdvertising()->setScanFilter(false,true);
    disabledPairing = true;
}

void CommunicationCarduinoNode::test() {
    clearWhitelist();
}

void CommunicationCarduinoNode::sendBLEPairingCode(int code) {
    EventMessage *eventMessage = new EventMessage(&Event::BLE_PAIRING_CODE, code);
    this->sendCanbusMessage(eventMessage);
    delete eventMessage;
}

void CommunicationCarduinoNode::onIdentity(NimBLEConnInfo info) {
    printlnWrapper("CommunicationCarduinoNode::onIdentity");
    if(!disabledPairing) {
        if(!NimBLEDevice::onWhiteList(info.getIdAddress())) {
            NimBLEDevice::whiteListAdd(info.getIdAddress());
            backupWhitelist();
        }
        clientAuthenticated(info);
    } else {
        bleServer->disconnect(info.getConnHandle());
        NimBLEDevice::deleteBond(info.getIdAddress());
        printlnWrapper("CommunicationCarduinoNode::onIdentity disabled pairing, cannot connect");
    }
}
