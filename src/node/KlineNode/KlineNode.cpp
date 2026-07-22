#include "KlineNode.h"

KlineNode::KlineNode(gpio_num_t tx_pin, gpio_num_t rx_pin): CarduinoNode(Node::KLINE.id) {
    NLOGD("KlineNode::KlineNode start");
    
    addSetting<bool>(&Setting::HANDLE_KLINE, true, true);
    //TODO: move this function in addSetting?
    restoreSettings();

    _tx_pin = tx_pin;
    _rx_pin = rx_pin;

    _uart_task_handle = nullptr;

    _kline = KLineKWP1281Lib{
        [this](unsigned long baud)                          { klineBegin(baud); },
        [this]()                                            { klineEnd(); },
        [this](uint8_t data)                                { klineSend(data); },
        [this](uint8_t *data, unsigned long timeout_ticks)  { return klineReceive(data, timeout_ticks); },
        static_cast<uint8_t>(tx_pin)   // pin TX per il bit-bang 5-baud
    };

    NLOGD("created _kline");

    _rx_sem = xSemaphoreCreateBinary();
    configASSERT(_rx_sem);

    NLOGD("created _rx_sem");

    // Task interno dedicato al polling periodico delle ECU configurate.
    // Priorità più bassa del task evento UART (KWP_TASK_PRI + 1) così la
    // ricezione byte-per-byte non viene mai ritardata dal polling applicativo.
    xTaskCreate(
        &KlineNode::kline_poll_task_trampoline,
        "kline_poll", KWP_TASK_STACK, this, KWP_TASK_PRI, nullptr
    );

    NLOGD("created task");

    this->_afterReadExecutors.addExecutor(std::make_shared<FuelConsumptionExecutor>());
    this->_afterReadExecutors.addExecutor(std::make_shared<CruiseExecutor>());

    NLOGD("KlineNode::KlineNode end");
}

void KlineNode::uart_event_loop() {
    NLOGD("KlineNode::uart_event_loop start");
    uart_event_t event;
    while (true) {
        if(_uart_queue != NULL) {
            NLOGD("KlineNode::uart_event_loop _uart_queue not null");
            if (xQueueReceive(_uart_queue, &event, portMAX_DELAY)) {
                if (event.type == UART_DATA || event.type == UART_BUFFER_FULL) {
                    xSemaphoreGive(_rx_sem);
                }
            }
        } else {
            NLOGD("KlineNode::uart_event_loop _uart_queue null");
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    NLOGD("KlineNode::uart_event_loop end");
}
// ─────────────────────────────────────────────
//  Task di polling interno
// ─────────────────────────────────────────────

void KlineNode::kline_poll_task_trampoline(void *arg) {
    NLOGD("KlineNode::kline_poll_task_trampoline start");
    static_cast<KlineNode *>(arg)->kline_poll_loop();
    NLOGD("KlineNode::kline_poll_task_trampoline end");
}

void KlineNode::kline_poll_loop() {
    const TickType_t period = pdMS_TO_TICKS(KLINE_POLL_INTERVAL_MS);
    while (true) {
        // UBaseType_t freeStack = uxTaskGetStackHighWaterMark(NULL); // NULL = task corrente
        // NLOGI("before readValues()");
        // NLOGI("Stack libero: %u words (%u bytes)", freeStack, freeStack * sizeof(StackType_t));
        // NLOGI("Heap libero: %u bytes, min storico: %u", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
        auto settingPtr = getSetting<bool>(&Setting::HANDLE_KLINE);
        if (settingPtr != nullptr && settingPtr->value) {
            readValues();
        }
        // NLOGI("after readValues()");
        // NLOGI("Stack libero: %u words (%u bytes)", freeStack, freeStack * sizeof(StackType_t));
        // NLOGI("Heap libero: %u bytes, min storico: %u", esp_get_free_heap_size(), esp_get_minimum_free_heap_size());
        vTaskDelay(period);
    }
}

// ─────────────────────────────────────────────
//  Gestione connessione con retry/backoff
// ─────────────────────────────────────────────

bool KlineNode::ensureConnected(KlineEcu *ecu) {
    if (ecu == nullptr) {
        return false;
    }

    // Cambio di ECU rispetto all'ultima a cui eravamo connessi/agganciati:
    // resettiamo lo stato di salute, dato che riguarda l'ECU precedente.
    if (_currentEcu.ecu != ecu) {
        _currentEcu.ecu               = ecu;
        _currentEcu.connState         = ConnState::DISCONNECTED;
        _currentEcu.consecFails       = 0;
        _currentEcu.backoffUntilTicks = 0;
    }

    if (_currentEcu.connState == ConnState::CONNECTED) {
        return true; // già connessi, niente da fare
    }

    if (_currentEcu.connState == ConnState::ERROR_BACKOFF) {
        if (xTaskGetTickCount() < _currentEcu.backoffUntilTicks) {
            return false; // non ancora scaduto il periodo di backoff
        }
        // Backoff scaduto: si può ritentare la connessione.
        _currentEcu.connState = ConnState::DISCONNECTED;
    }

    std::stringstream ss;
    // static_cast<int> è FONDAMENTALE, altrimenti ss inserisce il carattere ASCII corrispondente
    ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ecu->address);
    NLOGI("tentativo di connessione a ECU %s", ss.str().c_str());

    bool connected = _kline.attemptConnect(ecu->address, ecu->baud) == KLineKWP1281Lib::SUCCESS;

    if (connected) {
        _currentEcu.connState   = ConnState::CONNECTED;
        _currentEcu.consecFails = 0;
        NLOGI("connessione riuscita");
        return true;
    }

    // Connessione fallita: contiamo i fallimenti consecutivi su questa ECU.
    // Dopo KLINE_MAX_CONSEC_FAILURES tentativi falliti applichiamo un backoff,
    // per non saturare inutilmente il bus K-line con tentativi a vuoto.
    _currentEcu.consecFails++;
    if (_currentEcu.consecFails >= KLINE_MAX_CONSEC_FAILURES) {
        _currentEcu.connState         = ConnState::ERROR_BACKOFF;
        _currentEcu.backoffUntilTicks = xTaskGetTickCount() + pdMS_TO_TICKS(KLINE_BACKOFF_MS);

        

        std::stringstream ss;
        // static_cast<int> è FONDAMENTALE, altrimenti ss inserisce il carattere ASCII corrispondente
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(ecu->address);
        NLOGI("connessione fallita, backoff di %u ms su ECU %s", KLINE_BACKOFF_MS, ss.str().c_str());
    } else {
        _currentEcu.connState = ConnState::DISCONNECTED;
    }
    return false;
}

// ─────────────────────────────────────────────
//  Instradamento di una misura letta verso il CAN bus
// ─────────────────────────────────────────────

void KlineNode::dispatchMeasurement(ValueToRead *valueToRead, float value) {
    if (valueToRead->send) {
        EventBase *ev = EventRegistry::createByName(valueToRead->carstatus.name);
        ev->setFromFloat(value);

        std::ostringstream ss;
        ev->printValue(ss, false);            
        // NLOGI("dispatchMeasurement: %s, float: %.2f, event: %s",
        //     valueToRead->name, value, ss.str().c_str());

        sendMessage(Message(Priority::L.id, Node::MAIN.id, ev));
    }

    // Salvataggio nel variant in base al tipo specificato in carstatus
    if (valueToRead->carstatus.type->id == MessageType::BOOL.id) {
        // Se è un booleano, qualsiasi valore diverso da 0 diventa true
        _lastValues[valueToRead->id] = (value != 0.0f);
    } 
    else if (valueToRead->carstatus.type->id == MessageType::INT.id) { 
        // Se è un intero (es. i tuoi bitfield), facciamo il cast a int per non perdere i bit
        _lastValues[valueToRead->id] = static_cast<int>(value);
    } 
    else {
        // Di default (o se è MessageType::FLOAT.id), lo salviamo come float standard
        _lastValues[valueToRead->id] = value;
    }
}

void KlineNode::dispatchMeasurement(ValueToRead *valueToRead, std::string value) {
    // if (valueToRead->send) {
    //     EventBase *ev = EventRegistry::createByName(valueToRead->carstatus.name);
    //     ev->setFromFloat(value);

    //     std::ostringstream ss;
    //     ev->printValue(ss, false);            
    //     // NLOGI("dispatchMeasurement: %s, float: %.2f, event: %s",
    //     //     valueToRead->name, value, ss.str().c_str());

    //     sendMessage(Message(Priority::L.id, Node::MAIN.id, ev));
    // }

    if (valueToRead->carstatus.type->id == MessageType::TEXT.id) {
        _lastValues[valueToRead->id] = std::move(value); // finisce nel variant come std::string
    }
}

// ─────────────────────────────────────────────
//  Lettura di un singolo blocco (group) KWP1281
// ─────────────────────────────────────────────

bool KlineNode::readBlock(KlineEcu *ecu, uint8_t block) {
    uint8_t valuesByEcuBlockSize = ValueToRead::getValuesByEcuBlockSize(*ecu, block);
    if (valuesByEcuBlockSize == 0) {
        return true; // niente da leggere per questo blocco, non è un errore
    }

    ValueToRead **valuesByEcuBlock = ValueToRead::getValuesByEcuBlock(*ecu, block);
    if (valuesByEcuBlock == nullptr) {
        return true;
    }

    // Buffer per le misure: ogni misura occupa 3 byte, un blocco contiene fino a 4 misure.
    uint8_t measurements[3 * 4];
    uint8_t amount_of_measurements = 0;

    bool communicationOk = true;

    switch (_kline.readGroup(amount_of_measurements, block, measurements, sizeof(measurements))) {
        case KLineKWP1281Lib::ERROR:
            // Errore di comunicazione: la connessione va considerata persa.
            // Non riprovo gli altri blocchi su questa ECU in questo ciclo.
            NLOGI("KlineNode: errore di lettura sul blocco %u", block);
            communicationOk = false;
            break;

        case KLineKWP1281Lib::FAIL:
            // Il blocco richiesto non esiste su questa ECU: non è un errore di comunicazione,
            // semplicemente non ci sono valori da estrarre.
            NLOGI("KlineNode: il blocco %u non esiste su questa ECU", block);
            break;

        case KLineKWP1281Lib::SUCCESS:
            for (uint8_t i = 0; i < valuesByEcuBlockSize; i++) {
                ValueToRead *valueToRead = valuesByEcuBlock[i];
                if (valueToRead == nullptr) {
                    continue;
                }

                switch (KLineKWP1281Lib::getMeasurementType(valueToRead->groupIndex, amount_of_measurements, measurements, sizeof(measurements))) {
                    case KLineKWP1281Lib::VALUE: {
                        float value = static_cast<float>(KLineKWP1281Lib::getMeasurementValue(
                            valueToRead->groupIndex, amount_of_measurements, measurements, sizeof(measurements)));

                        if ((std::isnan)(value)) {
                            NLOGI("KlineNode: valore NaN per %s, scarto", valueToRead->name);
                            break; // non chiamare dispatchMeasurement con NaN
                        }

                        dispatchMeasurement(valueToRead, value);
                        break;
                    }

                    case KLineKWP1281Lib::TEXT: {
                        char text_string[16];
                        KLineKWP1281Lib::getMeasurementText(valueToRead->groupIndex, amount_of_measurements,
                            measurements, sizeof(measurements), text_string, sizeof(text_string));
                        // NLOGI("KlineNode: misura testuale: %s", text_string);
                        dispatchMeasurement(valueToRead, std::string(text_string));
                        break;
                    }

                    case KLineKWP1281Lib::UNKNOWN:
                    default:
                        // Indice di misura non presente in questo blocco: lo ignoriamo.
                        break;
                }
            }
            break;

        default:
            break;
    }

    delete[] valuesByEcuBlock;
    return communicationOk;
}

// ─────────────────────────────────────────────
//  Algoritmo principale: itera le ECU configurate
// ─────────────────────────────────────────────

void KlineNode::readValues() {
    // if (this->getSettingValue(&Setting::OTA_MODE)->value->boolValue) {
    //     // In modalità OTA la K-line va lasciata libera: se eravamo connessi, disconnettiamo
    //     // e resettiamo lo stato per ripartire da zero quando l'OTA terminerà.
    //     if (_currentEcu.connState == ConnState::CONNECTED) {
    //         _kline.disconnect();
    //     }
    //     _currentEcu = EcuRuntimeState{};
    //     return;
    // }

    if (!this->isEnabled) {
        return; // non leggere se il nodo non è abilitato
    }

    uint8_t ecusToReadSize = ValueToRead::getEcusToReadSize();
    KlineEcu **ecusToRead = ValueToRead::getEcusToRead();
    if (ecusToRead == nullptr) {
        return;
    }

    for (uint8_t ecuIndex = 0; ecuIndex < ecusToReadSize; ecuIndex++) {
        // esp_task_wdt_reset(); // nutri il WDT ad ogni ECU
        KlineEcu *ecu = ecusToRead[ecuIndex];
        if (ecu == nullptr) {
            continue;
        }

        uint8_t blockValuesByEcuSize = ValueToRead::getBlockValuesByEcuSize(*ecu);
        if (blockValuesByEcuSize == 0) {
            continue; // nessun blocco configurato per questa ECU
        }

        if (!ensureConnected(ecu)) {
            // Connessione non disponibile (fallita o in backoff): passiamo all'ECU successiva
            // senza bloccare l'intero ciclo di polling.
            continue;
        }

        uint8_t *blockValuesByEcu = ValueToRead::getBlockValuesByEcu(*ecu);
        if (blockValuesByEcu == nullptr) {
            continue;
        }

        bool ecuStillHealthy = true;
        for (uint8_t blockIndex = 0; blockIndex < blockValuesByEcuSize && ecuStillHealthy; blockIndex++) {
            ecuStillHealthy = readBlock(ecu, blockValuesByEcu[blockIndex]);
        }
        delete[] blockValuesByEcu;

        if (!ecuStillHealthy) {
            // Un blocco ha riportato un errore di comunicazione: la connessione è considerata
            // persa. Al prossimo giro ensureConnected() tenterà di riconnettersi (con backoff
            // se gli errori persistono), invece di restare agganciati a un'ECU morta.
            _currentEcu.connState = ConnState::DISCONNECTED;
            NLOGI("KlineNode: connessione persa con ECU %s", std::to_string(ecu->address).c_str());
        } else {
            _afterReadExecutors.execute(this);
        }
    }

    delete[] ecusToRead;
}

void KlineNode::klineBegin(unsigned long baud) {
    NLOGD("KlineNode::klineBegin start");

    if (_uart_task_handle != nullptr) {
        vTaskDelete(_uart_task_handle);
        _uart_task_handle = nullptr;
    }

    uart_driver_delete(_uart);

    const uart_config_t uart_cfg = {
        .baud_rate  = static_cast<int>(baud),
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SCLK_DEFAULT,
        .flags      = {}
    };

    ESP_ERROR_CHECK(uart_param_config(_uart, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(_uart, _tx_pin, _rx_pin,
                                    UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(_uart, UART_BUF_SIZE, 0,
                                            10, &_uart_queue, 0));

    // Soglia FIFO a 1 byte: l'interrupt scatta appena arriva il primo byte.
    ESP_ERROR_CHECK(uart_set_rx_full_threshold(_uart, 1));

    xSemaphoreTake(_rx_sem, 0);

    // Passo direttamente 'this' senza allocare strutture intermedie
    xTaskCreate(
        [](void *arg) {
            auto *self = static_cast<KlineNode *>(arg);
            self->uart_event_loop(); // Userà internamente self->_uart_queue
        },
        "uart_evt",
        4096,
        this,
        KWP_TASK_PRI + 1,
        &_uart_task_handle
    );
    
    NLOGD("KlineNode::klineBegin end");
};

void KlineNode::klineEnd() {
    NLOGD("KlineNode::klineEnd start");

    if (_uart_task_handle != nullptr) {
        vTaskDelete(_uart_task_handle); // Ferma il task "uart_evt" istantaneamente
        _uart_task_handle = nullptr;    // Ripristina il puntatore a null
        NLOGD("Vecchio uart_task eliminato con successo");
    }

    uart_driver_delete(_uart);
    _uart_queue = nullptr;
    NLOGD("KlineNode::klineEnd end");
};

void KlineNode::klineSend(uint8_t data) {
    uart_write_bytes(_uart, reinterpret_cast<const char *>(&data), 1);
};

bool KlineNode::klineReceive(uint8_t *data, unsigned long timeout_ticks) {
    int bytes_read = uart_read_bytes(_uart, data, 1, 0);
    if (bytes_read == 1) {
        return true;
    }

    if (xSemaphoreTake(_rx_sem, static_cast<TickType_t>(timeout_ticks)) != pdTRUE) {
        return false; // timeout
    }

    bytes_read = uart_read_bytes(_uart, data, 1, 0);
    return bytes_read == 1;
};