#include "MainNode.h"

MainNode::MainNode(): CarduinoNode(Node::MAIN.id, true), I2cNode() {
    NLOGD("MainNode::MainNode called");

    _canExecutor.addExecutor(new BootExecutor());
    _canExecutor.addExecutor(new HelloTrackerExecutor());
    _canExecutor.addExecutor(new MainNodeCanEvent());
    
    _serialExecutor.addExecutor(new SwcPairingEvent());

    _buzzer.init(GPIO_NUM_2);
    configTemt6000();
    configAht();
    configBmp();
    configSwc();
    configEncoder();
    configSpeedWarning();

    // Chiamiamo l'inizializzazione dei dispositivi adesso che l'oggetto è pronto!
    initI2cDevices();

    this->enable();
}

void MainNode::configEncoder() {
    _speedLimitEditTimer = xTimerCreate("spdLimEdit",
                                     pdMS_TO_TICKS(SPEED_LIMIT_EDIT_TIMEOUT_MS),
                                     pdFALSE, this, &MainNode::speedLimitEditTimeoutCallback);
    if (_speedLimitEditTimer == nullptr) {
        NLOGE("speedLimitEditTimer xTimerCreate FAILED");
    }

    _encoder.setOnSingleClick([this]{
        NLOGI("SWC SINGLE click");
        pressSwcAsync(findSwcMapping(SwcPattern::SINGLE_CLICK)->channel, SWC_PRESS_INTERVAL);
    });
    _encoder.setOnDoubleClick([this]{
        NLOGI("SWC DOUBLE click");
        pressSwcAsync(findSwcMapping(SwcPattern::DOUBLE_CLICK)->channel, SWC_PRESS_INTERVAL);
    });
    _encoder.setOnTripleClick([this]{
        _speedLimitSetMode = !_speedLimitSetMode;
        NLOGI("Speed limit set mode: %d", _speedLimitSetMode);

        if (_speedLimitSetMode) {
            NLOGI("Speed limit: %d", _speedWarn.getLimit());
            if(_speedWarn.getLimit() == 0) {
                NLOGI("last speed: %d", lastSpeed);
                _speedWarn.setLimit(lastSpeed);
            }
            _buzzer.playToneAsync(ToneType::MODE_ENTER);
            if (_speedLimitEditTimer) xTimerStart(_speedLimitEditTimer, 0);
        } else {
            if (_speedLimitEditTimer) xTimerStop(_speedLimitEditTimer, 0);
            exitSpeedLimitEditMode();
        }

        const SwcMapping *m = findSwcMapping(SwcPattern::TRIPLE_CLICK);
        if (m) pressSwcAsync(m->channel, SWC_PRESS_INTERVAL);
    });

    _encoder.setOnRotate([this](int32_t diff){
        if (_speedLimitSetMode) {
            int32_t nl = (int32_t)_speedWarn.getLimit() + (diff > 0 ? 5 : -5);
            _speedWarn.setLimit(nl < 0 ? 0 : nl);
            NLOGI("Speed limit -> %u", _speedWarn.getLimit());
            if (_speedLimitEditTimer) xTimerReset(_speedLimitEditTimer, 0); // riazzera countdown
            return;
        }
        NLOGI("SWC ROTATE diff=%ld", (long)diff);
        pressSwcAsync(findSwcMapping(diff > 0 ? SwcPattern::CW_ROTATION : SwcPattern::CCW_ROTATION)->channel,
                    SWC_PRESS_INTERVAL);
    });

    _encoder.setOnRotateHeld([this](int32_t diff){
        if (_speedLimitSetMode) {
            int32_t nl = (int32_t)_speedWarn.getLimit() + (diff > 0 ? 20 : -20);
            _speedWarn.setLimit(nl < 0 ? 0 : nl);
            NLOGI("Speed limit (fast) -> %u", _speedWarn.getLimit());
            if (_speedLimitEditTimer) xTimerReset(_speedLimitEditTimer, 0);
            return;
        }
        NLOGI("SWC ROTATE+HOLD diff=%ld", (long)diff);
    });

    _encoder.setOnLongPress([this]{
        auto *ev = static_cast<EventMulti<uint8_t>*>(EventRegistry::createById(EV_LONG_PRESS));
        std::get<0>(ev->values) = 0;
        Message m = Message(Priority::L.id, Node::BROADCAST.id, ev);
        sendSerialMessage(m);
    });

    _encoder.init(GPIO_ENCODER_A, GPIO_ENCODER_B, GPIO_BUTTON);
}

void MainNode::configSpeedWarning() {
    _speedWarn.init([this]{ _buzzer.playToneAsync(1200, 150, 100, 3); });
    _speedWarn.setEnabled(true);
    _speedWarn.setRepeatIntervalMs(30000);
    _speedWarn.setHysteresisKmh(10);
}

void MainNode::configSwc() {
    // stesso bus I2C già usato da AHT/BMP280 (I2cNode)
    ESP_ERROR_CHECK(_swc.init(I2C_NUM_0, 0x21, DEFAULT_I2C_SDA_PIN, DEFAULT_I2C_SCL_PIN)); // adatta SDA/SCL ai tuoi già usati
}


void MainNode::configBmp() {
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&bpm_dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&bpm_dev, BMP280_I2C_ADDRESS_1, I2C_NUM_0, DEFAULT_I2C_SDA_PIN, DEFAULT_I2C_SCL_PIN));
    ESP_ERROR_CHECK(bmp280_init(&bpm_dev, &params));

    NLOGD("Sensore BMP inizializzato.");

    startRepeatingTask("bmp280_read", 15000, [this]() {
        float temperature;
        float pressure;
        float humidity;

        if (bmp280_read_float(&bpm_dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            NLOGI("Temperature/pressure reading failed");
            return;
        }

        uint16_t pressure_int = (uint16_t)(pressure / 100);

        
        auto *ev = static_cast<EventMulti<uint16_t> *>(EventRegistry::createById(EV_INTERNAL_PRESSURE));
        std::get<0>(ev->values) = pressure_int;
        Message m = Message(Priority::L.id, Node::BROADCAST.id, ev);

        sendMessage(m);
        sendSerialMessage(m);
    });
}

void MainNode::configAht() {
    // Azzera completamente la struttura per ripulire la memoria da valori residui/spazzatura
    memset(&aht_dev, 0, sizeof(aht_dev));

    // --- Configurazione del sensore AHT ---
    aht_dev.mode = AHT_MODE_NORMAL;
    aht_dev.type = AHT_TYPE_AHT20;

    // Ora i2cdev troverà il puntatore del mutex a NULL e lo allocherà correttamente in RAM
    ESP_ERROR_CHECK(aht_init_desc(&aht_dev, AHT_I2C_ADDRESS_GND, I2C_NUM_0, DEFAULT_I2C_SDA_PIN, DEFAULT_I2C_SCL_PIN));
    ESP_ERROR_CHECK(aht_init(&aht_dev));

    NLOGD("Sensore AHT inizializzato.");
    

    startRepeatingTask("aht20_read", 15000, [this]() {
        float temperature;
        float humidity;

        if (aht_get_data(&aht_dev, &temperature, &humidity) != ESP_OK)
        {
            NLOGI("Temperature/humidity reading failed");
            return;
        }
        
        auto *ev = static_cast<EventMulti<float> *>(EventRegistry::createById(EV_INTERNAL_TEMPERATURE));
        std::get<0>(ev->values) = temperature;
        Message m = Message(Priority::L.id, Node::BROADCAST.id, ev);

        sendMessage(m);
        sendSerialMessage(m);
    });
}

void MainNode::configTemt6000() {
    NLOGD("MainNode::configTemt6000 called");

    // 1. Inizializzazione dell'Unità ADC
    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = TEMT6000_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));

    // 2. Configurazione del Canale ADC
    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, TEMT6000_ADC_CHANNEL, &config));

    // 3. Configurazione della Calibrazione
    adc_cali_handle_t cali_handle = NULL;
    bool do_calibration = false;

    #if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = TEMT6000_ADC_UNIT,
        .chan = TEMT6000_ADC_CHANNEL,
        .atten = ADC_ATTEN_DB_2_5,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    if (adc_cali_create_scheme_curve_fitting(&cali_config, &cali_handle) == ESP_OK) {
        do_calibration = true;
    }
    #endif

    static constexpr int   SAMPLES_NUM  = 10;     // n. campioni per media
    static constexpr float LOAD_OHM     = 10000.0f; // resistore di carico (10kΩ)
    static constexpr float UA_PER_LUX   = 2.0f;    // datasheet: 2µA -> 1 lux

    startRepeatingTask("temt6000_read", 1000, [this, adc_handle, cali_handle, do_calibration]() {
        int adc_raw = 0;
        long voltage_sum_mv = 0;
        int valid_samples = 0;

        for (int i = 0; i < SAMPLES_NUM; ++i) {
            if (adc_oneshot_read(adc_handle, TEMT6000_ADC_CHANNEL, &adc_raw) != ESP_OK) {
                NLOGI("adc_oneshot_read error");
                continue;
            }

            if (do_calibration) {
                int voltage_mv = 0;
                if (adc_cali_raw_to_voltage(cali_handle, adc_raw, &voltage_mv) == ESP_OK) {
                    voltage_sum_mv += voltage_mv;
                    valid_samples++;
                }
            }
        }

        if (do_calibration && valid_samples > 0) {
            float voltage_avg_mv = (float)voltage_sum_mv / valid_samples;

            // I = V / R  (mV / ohm -> mA, poi convertito in µA)
            float current_uA = (voltage_avg_mv / LOAD_OHM) * 1000.0f;

            // lux = corrente(µA) / 2
            float lux = current_uA / UA_PER_LUX;
            int lux_int = (int)(lux + 0.5f);

            // percentuale rispetto al fondo scala 3300 mV (indicativo)
            float light_percent = (voltage_avg_mv / 3300.0f) * 100.0f;
            if (light_percent > 100.0f) light_percent = 100.0f;

            NLOGD("TEMT6000: %.1f mV | %.2f lux | %.1f%%", voltage_avg_mv, lux, light_percent);

        
            auto *ev = static_cast<EventMulti<uint16_t> *>(EventRegistry::createById(EV_INTERNAL_LUMINANCE));
            std::get<0>(ev->values) = lux_int;
            Message m = Message(Priority::L.id, Node::BROADCAST.id, ev);

            sendMessage(m);
            sendSerialMessage(m);
        } else {
            NLOGD("TEMT6000: calibrazione non disponibile o lettura fallita");
        }
    });
}

void MainNode::initI2cDevices() {
    // Inizializza eventuali dispositivi I2C aggiuntivi qui
    NLOGD("MainNode: initI2cDevices called");
}

void MainNode::enable() {
    // if (isEnabled) {
    //     NLOGD("MainNode::enable called, ma il nodo è già enabled: no-op");
    //     return;
    // }

    CarduinoNode::enable();

    stopRepeatingTask(HELLO_TASK_ID);
    NLOGI("MainNode::enable: nodo MAIN enabled (id=%u)", static_cast<unsigned>(_id));

    sendMessage(Message(Priority::L.id, Node::BROADCAST.id, EventRegistry::createById(EV_GET_HELLOS)));

    // delayTask(5000, [this](){
    //     // startSwcPairing();
    //     for(uint8_t i = 0; i <= 100; i = i + 10) {
    //         lastSpeed = i;
    //         _speedWarn.onSpeedUpdate(i);
    //         NLOGI("Set speed to %i", i);
    //         vTaskDelay(pdMS_TO_TICKS(5000));
    //     }
    // });
}

void MainNode::recordHello(uint8_t senderId) {
    _knownNodes[senderId] = syncedMillis();
    NLOGD("MainNode: HELLO ricevuto da id=%u, nodi noti=%u", static_cast<unsigned>(senderId), static_cast<unsigned>(_knownNodes.size()));
}

bool MainNode::hasSeenNode(uint8_t nodeId) const {
    return _knownNodes.find(nodeId) != _knownNodes.end();
}

uint32_t MainNode::lastHelloMillis(uint8_t nodeId) const {
    auto it = _knownNodes.find(nodeId);
    return it != _knownNodes.end() ? it->second : 0;
}

void MainNode::handleTimeSyncRequest(uint8_t requesterId) {
    // Eseguito sul nodo MAIN (è l'unico a cui arrivano richieste con
    // destination==MAIN). T2 va preso il più vicino possibile alla ricezione,
    // T3 il più vicino possibile all'invio, per minimizzare il tempo di
    // elaborazione incluso per errore nella stima.
    uint32_t t2 = localMillis();

    Message resp(Priority::H.id, requesterId,
                 new EventMulti<uint32_t, uint32_t>(EV_TIME_SYNC_RESPONSE, "TIME_SYNC_RESPONSE"));
    std::get<0>(static_cast<EventMulti<uint32_t,uint32_t>*>(resp.event)->values) = t2;
    
    uint32_t t3 = localMillis();
    std::get<1>(static_cast<EventMulti<uint32_t,uint32_t>*>(resp.event)->values) = t3;

    sendMessage(resp);
}

void MainNode::startTimeSync() {
    return;
}

uint32_t MainNode::syncedMillis() const {
    return localMillis();
}

static void pressTask(void* arg) {
    PressParams* p = static_cast<PressParams*>(arg);
    p->ctrl->pressOneShotAsync(p->channel, p->holdMs);
    delete p;
    vTaskDelete(NULL);
}

void MainNode::pressSwcAsync(uint8_t channel, uint32_t holdMs) {
    auto* params = new PressParams{&_swc, channel, holdMs};
    xTaskCreate(pressTask, "swc_press", configMINIMAL_STACK_SIZE * 2, params, 5, NULL);
}

void MainNode::startSwcPairing() {
    if (swcPairing) return;
    swcPairing = true;
    xTaskCreate(swcPairingTask, "swc_pairing", 4096, this, 5, nullptr);
}

void MainNode::swcPairingTask(void* param) {
    auto* self = static_cast<MainNode*>(param);
    vTaskDelay(pdMS_TO_TICKS(SWC_FIRST_WAITING_PAIRING_INTERVAL));

    self->_buzzer.playToneAsync(ToneType::WARNING);

    for (uint8_t i = 0; i < SWC_MAPPINGS_SIZE; i++) {
        const SwcMapping& m = SWC_MAPPINGS[i];

        self->_buzzer.playToneAsync(ToneType::INFO);
        NLOGI("MainNode SWC pairing: %s (channel %u, pattern %u)",
              EventRegistry::getName(m.eventId), m.channel, (uint8_t) m.pattern);

        self->pressSwcAsync(m.channel, SWC_PAIRING_INTERVAL);

        vTaskDelay(pdMS_TO_TICKS(SWC_PAIRING_INTERVAL + SWC_WAITING_PAIRING_INTERVAL));
    }

    NLOGI("MainNode SWC pairing finished");
    // self->playTone(&Event::WARNING_SEVERITY_MEDIUM);
    self->swcPairing = false;
    self->_buzzer.playToneAsync(ToneType::WARNING);
    vTaskDelete(nullptr);
}

void MainNode::speedLimitEditTimeoutCallback(TimerHandle_t t) {
    auto* self = static_cast<MainNode*>(pvTimerGetTimerID(t));
    NLOGI("Speed limit edit timeout -> exit");
    self->exitSpeedLimitEditMode();
}

void MainNode::exitSpeedLimitEditMode() {
    if (!_speedLimitSetMode) return;
    _speedLimitSetMode = false;
    _buzzer.playToneAsync(ToneType::MODE_EXIT);

    auto *ev = static_cast<EventMulti<uint8_t>*>(EventRegistry::createById(EV_SPEED_LIMIT_SET));
    std::get<0>(ev->values) = _speedWarn.getLimit();
    Message m = Message(Priority::L.id, Node::BROADCAST.id, ev);
    sendSerialMessage(m);
}