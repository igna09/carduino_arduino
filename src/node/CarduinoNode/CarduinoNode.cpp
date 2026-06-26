#include "CarduinoNode.h"

CarduinoNode::CarduinoNode(uint8_t id): SettingBase(), UdpLogSender() {
    NLOGI("CarduinoNode::CarduinoNode start");

    _id = id;

    addSetting(&Setting::OTA_MODE, false);

    // Configure TWAI node
    twai_onchip_node_config_t node_config = {
        .io_cfg = {
            .tx = DEFAULT_CAN_TX_PIN,
            .rx = DEFAULT_CAN_RX_PIN,
            .quanta_clk_out = GPIO_NUM_NC,
            .bus_off_indicator = GPIO_NUM_NC,
        },
        .bit_timing = {
            .bitrate = TWAI_BITRATE,
        },
        .fail_retry_cnt = 3,
        .tx_queue_depth = TWAI_QUEUE_DEPTH,
    };

    // Create TWAI node
    ESP_ERROR_CHECK(twai_new_node_onchip(&node_config, &_twai_node));

    // Pool RX e semafori, creati prima di registrare i callback (l'ISR potrebbe
    // scattare subito dopo twai_node_enable, quindi tutto deve essere pronto).
    setupRxPool();

    // Registra i callback ISR (rx_done / state_change / error) passando 'this'
    // come user_ctx, così i metodi statici possono risalire all'istanza.
    registerTwaiCallbacks();

    // Enable TWAI node
    ESP_ERROR_CHECK(twai_node_enable(_twai_node));
    NLOGI("TWAI node started successfully");

    // Task dedicato alla recovery da bus_off, per-nodo, priorità bassa.
    startRecoveryTask();

    // Task dedicato al drain del pool RX e dispatch verso Message::fromCanFrame.
    startRxTask();

    // Annuncio spontaneo a MAIN: ogni nodo che NON è MAIN, appena pronto a
    // ricevere/inviare sul bus, si annuncia mandando HELLO con il proprio id
    // nel payload. MAIN non manda HELLO a se stesso: riceve solo annunci
    // (sia spontanei come questo, sia in risposta a un suo GET_HELLOS).
    // MAIN risponderà con ENABLE (gestito in CarduinoNodeCanEvent), che a
    // sua volta fa scattare startTimeSync() su questo nodo.
    if (_id != Node::MAIN.id) {
        Message hello(Priority::L.id, Node::MAIN.id,
                      new EventMulti<uint8_t>(EV_HELLO, "HELLO"));
        std::get<0>(static_cast<EventMulti<uint8_t>*>(hello.event)->values) = _id;
        sendMessage(hello);
        NLOGI("HELLO inviato a MAIN (id=%u)", static_cast<unsigned>(_id));
    }

    _serialExecutor.addExecutor(new CarduinoNodeSerialWriteSetting());

    NLOGI("CarduinoNode::CarduinoNode end");
}

std::string CarduinoNode::name() {
    return std::string(Node::getValueById(_id)->name);
}

void CarduinoNode::onMessageReceived(std::function<void(Message*)> handler) {
    _onMessage = std::move(handler);
}

// ============================================================================
// Setup RX pool / callback / task ausiliari
// ============================================================================

void CarduinoNode::setupRxPool() {
    _freePoolSemaphore = xSemaphoreCreateCounting(CAN_RX_POOL_DEPTH, CAN_RX_POOL_DEPTH);
    _rxResultSemaphore = xSemaphoreCreateCounting(CAN_RX_POOL_DEPTH, 0);
    assert(_freePoolSemaphore != nullptr);
    assert(_rxResultSemaphore != nullptr);

    _rxPool = static_cast<CanRxSlot*>(calloc(CAN_RX_POOL_DEPTH, sizeof(CanRxSlot)));
    assert(_rxPool != nullptr);

    for (int i = 0; i < CAN_RX_POOL_DEPTH; i++) {
        _rxPool[i].frame.buffer = _rxPool[i].data;
        _rxPool[i].frame.buffer_len = sizeof(_rxPool[i].data);
    }

    NLOGI("RX pool inizializzato: %d slot", CAN_RX_POOL_DEPTH);
}

void CarduinoNode::registerTwaiCallbacks() {
    twai_event_callbacks_t callbacks = {
        .on_rx_done = CarduinoNode::onRxDoneCallback,
        .on_state_change = CarduinoNode::onStateChangeCallback,
        .on_error = CarduinoNode::onErrorCallback,
    };
    ESP_ERROR_CHECK(twai_node_register_event_callbacks(_twai_node, &callbacks, this));
}

void CarduinoNode::startRecoveryTask() {
    // Nome task troncato a 15 char + terminatore: limite consigliato FreeRTOS
    // (configMAX_TASK_NAME_LEN è spesso 16). "CAN_REC_" + id a 3 cifre ci sta.
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "CAN_REC_%u", static_cast<unsigned>(_id));

    xTaskCreate(
        CarduinoNode::recoveryTaskEntry,
        taskName,
        4096,
        this,
        2, // priorità bassa
        &_recoveryTaskHdl
    );
}

void CarduinoNode::startRxTask() {
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "CAN_RX_%u", static_cast<unsigned>(_id));

    xTaskCreate(
        CarduinoNode::rxTaskEntry,
        taskName,
        4096,
        this,
        6,
        &_rxTaskHdl
    );
}

// ============================================================================
// Callback ISR (contesto interrupt: nessuna chiamata bloccante, solo update
// di flag/semafori e notify del task dedicato)
// ============================================================================

bool IRAM_ATTR CarduinoNode::onErrorCallback(twai_node_handle_t handle,
                                              const twai_error_event_data_t *edata,
                                              void *user_ctx) {
    ESP_EARLY_LOGW("CAN", "bus error: 0x%x", edata->err_flags.val);
    return false;
}

bool IRAM_ATTR CarduinoNode::onStateChangeCallback(twai_node_handle_t handle,
                                                    const twai_state_change_event_data_t *edata,
                                                    void *user_ctx) {
    auto *self = static_cast<CarduinoNode*>(user_ctx);

    // Aggiorniamo solo il flag atomico: niente recovery qui, la gestisce il
    // task dedicato per evitare chiamate bloccanti in contesto ISR.
    BaseType_t woken = pdFALSE;
    if (edata->new_sta == TWAI_ERROR_BUS_OFF) {
        self->_busOff.store(true, std::memory_order_relaxed);
        if (self->_recoveryTaskHdl != nullptr) {
            vTaskNotifyGiveFromISR(self->_recoveryTaskHdl, &woken);
        }
    } else if (edata->new_sta == TWAI_ERROR_ACTIVE) {
        self->_busOff.store(false, std::memory_order_relaxed);
    }
    return (woken == pdTRUE);
}

bool IRAM_ATTR CarduinoNode::onRxDoneCallback(twai_node_handle_t handle,
                                               const twai_rx_done_event_data_t *edata,
                                               void *user_ctx) {
    auto *self = static_cast<CarduinoNode*>(user_ctx);
    BaseType_t woken = pdFALSE;

    if (xSemaphoreTakeFromISR(self->_freePoolSemaphore, &woken) != pdTRUE) {
        // Pool pieno: il task RX non sta consumando abbastanza in fretta.
        return (woken == pdTRUE);
    }

    if (twai_node_receive_from_isr(handle, &self->_rxPool[self->_rxWriteIdx].frame) == ESP_OK) {
        self->_rxWriteIdx = (self->_rxWriteIdx + 1) % CAN_RX_POOL_DEPTH;
        xSemaphoreGiveFromISR(self->_rxResultSemaphore, &woken);
    }
    return (woken == pdTRUE);
}

// ============================================================================
// Recovery task (per-nodo): stesso schema di CanTest.cpp, applicato all'istanza
// ============================================================================

void CarduinoNode::recoveryTaskEntry(void *pvParameters) {
    auto *self = static_cast<CarduinoNode*>(pvParameters);
    const TickType_t safety_wait = pdMS_TO_TICKS(CAN_RECOVERY_SAFETY_MS);

    while (1) {
        // Sveglia su notify dall'ISR, oppure al massimo dopo safety_wait per
        // restare robusti anche a una notify persa.
        ulTaskNotifyTake(pdTRUE, safety_wait);

        if (!self->_busOff.load(std::memory_order_relaxed)) {
            continue;
        }

        if (self->_recoveryInProgress.exchange(true, std::memory_order_relaxed)) {
            continue; // recovery già in corso per questo nodo
        }

        NLOGW("Bus in BUS_OFF, avvio recovery...");
        esp_err_t err = twai_node_recover(self->_twai_node);
        if (err != ESP_OK) {
            NLOGE("twai_node_recover() fallita: %s", esp_err_to_name(err));
            self->_recoveryInProgress.store(false, std::memory_order_relaxed);
            continue;
        }

        int waited_ms = 0;
        bool recovered = false;
        twai_node_status_t node_status;
        while (waited_ms < CAN_RECOVERY_WAIT_MS) {
            vTaskDelay(pdMS_TO_TICKS(CAN_RECOVERY_STEP_MS));
            waited_ms += CAN_RECOVERY_STEP_MS;

            if (twai_node_get_info(self->_twai_node, &node_status, NULL) == ESP_OK &&
                node_status.state == TWAI_ERROR_ACTIVE) {
                self->_busOff.store(false, std::memory_order_relaxed);
                NLOGI("Recovery completata, nodo error_active dopo %d ms", waited_ms);
                recovered = true;
                break;
            }
        }

        if (!recovered) {
            NLOGE("Recovery non completata dopo %d ms, nodo ancora in errore", CAN_RECOVERY_WAIT_MS);
            // _busOff resta true: il prossimo wake (notify o safety_wait) ritenterà.
        }

        self->_recoveryInProgress.store(false, std::memory_order_relaxed);
    }
}

// ============================================================================
// RX task (per-nodo): drena il pool e dispatcha verso Message::fromCanFrame
// ============================================================================

void CarduinoNode::rxTaskEntry(void *pvParameters) {
    auto *self = static_cast<CarduinoNode*>(pvParameters);

    while (1) {
        if (xSemaphoreTake(self->_rxResultSemaphore, portMAX_DELAY) == pdTRUE) {
            int current_idx = self->_rxReadIdx;
            twai_frame_t *frame = &self->_rxPool[current_idx].frame;

            // Ricostruisce il Message dal frame CAN ricevuto. fromCanFrame
            // ritorna nullptr se l'eventId codificato nell'ID CAN non è
            // registrato in EventRegistry, o se il payload è più corto di
            // quanto l'evento si aspetta: in entrambi i casi scartiamo
            // silenziosamente (con log) e liberiamo lo slot.
            Message* msg = Message::fromCanFrame(frame->header.id, frame->buffer, frame->header.dlc);

            if (msg == nullptr) {
                NLOGW("RX: frame id=0x%x dlc=%d non decodificabile, scartato",
                      static_cast<unsigned>(frame->header.id), frame->header.dlc);
            } else if (self->_onMessage) {
                // Il chiamante diventa owner del Message* e deve fare delete.
                self->_onMessage(msg);
            } else {
                // Nessun handler registrato: evitiamo il leak.
                NLOGD("RX: %s (nessun handler registrato)", msg->toString().c_str());
                delete msg;
            }

            self->_rxReadIdx = (self->_rxReadIdx + 1) % CAN_RX_POOL_DEPTH;
            xSemaphoreGive(self->_freePoolSemaphore);
        }
    }
}

// ============================================================================
// TX
// ============================================================================

void CarduinoNode::sendMessage(const Message& m) {
    uint16_t id = 0;
    uint8_t payload[8];
    uint8_t dlc = m.toCanFrame(id, payload, sizeof(payload));

    sendByte(m.canId(), dlc, payload);
}

void CarduinoNode::sendSerialMessage(const Message& m) {
    m.print(std::cout, true);
}

void CarduinoNode::sendByte(uint16_t messageId, int len, uint8_t *buf) {
    // Se il bus è in bus_off, il recovery task per questo nodo se ne sta già
    // occupando in background: non blocchiamo qui, scartiamo il frame.
    if (_busOff.load(std::memory_order_relaxed)) {
        NLOGW("sendByte: bus in BUS_OFF, frame id=0x%x scartato", messageId);
        return;
    }

    twai_frame_t tx_frame = {
        .header = {
            .id = messageId,
        },
        .buffer = buf,
        .buffer_len = static_cast<size_t>(len),
    };

    esp_err_t err = twai_node_transmit(_twai_node, &tx_frame, pdMS_TO_TICKS(50));
    if (err != ESP_OK) {
        NLOGE("twai_node_transmit() fallita per id=0x%x: %s", messageId, esp_err_to_name(err));
    }
};

void CarduinoNode::delayTask(unsigned long millisec, std::function<void()> lambda) {
    // Passiamo i ms e la lambda al task FreeRTOS
    struct TaskArgs {
        unsigned long millisec;
        std::function<void()> lambda;
    };

    auto *args = new TaskArgs{millisec, lambda};

    xTaskCreate(
        [](void *param) {
            auto *p = static_cast<TaskArgs *>(param);

            // Attende il tempo richiesto senza bloccare l'ESP32
            vTaskDelay(pdMS_TO_TICKS(p->millisec));

            // Esegue la lambda
            p->lambda();

            // Pulisce la memoria ed elimina il task autonomamente
            delete p;
            vTaskDelete(NULL);
        },
        "delayed_lambda",
        3072, // Stack size
        args,
        1,    // Priorità bassa
        nullptr
    );
}

void CarduinoNode::startRepeatingTask(const std::string& id, uint32_t periodMs, std::function<void()> lambda, uint32_t stackSize, UBaseType_t priority) {
    stopRepeatingTask(id);

    auto *ctx = new RepeatingTaskCtx{std::move(lambda), periodMs, id};

    TaskHandle_t handle = nullptr;
    xTaskCreate(
        [](void *param) {
            auto *p = static_cast<RepeatingTaskCtx *>(param);
            while (!p->stop) {
                p->fn();
                NLOGD("Repeating task %d executed, sleeping for %d ms\n", p->id, p->periodMs);
                vTaskDelay(pdMS_TO_TICKS(p->periodMs));
            }
            delete p;
            vTaskDelete(NULL);
        },
        id.c_str(),
        3072,
        ctx,
        1,
        &handle
    );

    tasks_[id] = {handle, ctx};
}

// Stoppa un singolo task ripetitivo dato l'id
void CarduinoNode::stopRepeatingTask(const std::string& id) {
    auto it = tasks_.find(id);
    if (it == tasks_.end()) return;

    it->second.ctx->stop = true; // segnala lo stop in modo cooperativo

    // Aspetta che il task termini da solo (si autodistrugge)
    tasks_.erase(it);
}

void CarduinoNode::stopAllRepeatingTasks() {
    for (auto& [id, entry] : tasks_) {
        entry.ctx->stop = true;
    }
    tasks_.clear();
}


// ============================================================================
// Time sync
// ============================================================================

bool CarduinoNode::isTimeSynced() const {
    return _timeSynced.load(std::memory_order_relaxed);
}

uint32_t CarduinoNode::syncedMillis() const {
    if (!_timeSynced.load(std::memory_order_relaxed)) {
        // Fallback silenzioso sul clock locale (equivalente a offset=0).
        // Logghiamo una volta sola per non spammare se il chiamante interroga
        // questo metodo a ripetizione prima che il sync vada a buon fine.
        if (!_unsyncedWarnLogged) {
            const_cast<CarduinoNode*>(this)->_unsyncedWarnLogged = true;
            NLOGW("syncedMillis() chiamato prima del time sync: ritorno clock locale non corretto");
        }
        return localMillis();
    }
    // _timeOffsetMs può essere negativo: il cast a int64_t evita underflow
    // strani vicino a 0 prima di tornare a uint32_t (wrap atteso a ~49 giorni,
    // coerente per entrambi i lati visto che usano la stessa size).
    return static_cast<uint32_t>(static_cast<int64_t>(localMillis()) + _timeOffsetMs);
}

void CarduinoNode::startTimeSync() {
    if (_syncPending.exchange(true, std::memory_order_relaxed)) {
        NLOGW("startTimeSync: sync già in corso, richiesta ignorata");
        return;
    }

    _syncT1Ms = localMillis();
    NLOGI("Time sync: invio TIME_SYNC_REQUEST a MAIN (T1=%u ms)", static_cast<unsigned>(_syncT1Ms));

    Message req(Priority::H.id, Node::MAIN.id,
                new EventMulti<uint8_t>(EV_TIME_SYNC_REQUEST, "TIME_SYNC_REQUEST"));
    std::get<0>(static_cast<EventMulti<uint8_t>*>(req.event)->values) = _id;

    sendMessage(req);

    // Timeout difensivo: se la response non arriva, libera _syncPending così
    // un retry successivo (manuale o a un nuovo ENABLE) non resta bloccato.
    delayTask(TIME_SYNC_TIMEOUT_MS, [this]() {
        if (_syncPending.exchange(false, std::memory_order_relaxed)) {
            NLOGE("Time sync: timeout dopo %d ms, nessuna TIME_SYNC_RESPONSE ricevuta", TIME_SYNC_TIMEOUT_MS);
        }
    });
}

void CarduinoNode::handleTimeSyncRequest(uint8_t requesterId) {
    // Eseguito sul nodo MAIN (è l'unico a cui arrivano richieste con
    // destination==MAIN). T2 va preso il più vicino possibile alla ricezione,
    // T3 il più vicino possibile all'invio, per minimizzare il tempo di
    // elaborazione incluso per errore nella stima.
    uint32_t t2 = localMillis();
    uint32_t t3 = localMillis();

    Message resp(Priority::H.id, requesterId,
                 new EventMulti<uint32_t, uint32_t>(EV_TIME_SYNC_RESPONSE, "TIME_SYNC_RESPONSE"));
    std::get<0>(static_cast<EventMulti<uint32_t,uint32_t>*>(resp.event)->values) = t2;
    std::get<1>(static_cast<EventMulti<uint32_t,uint32_t>*>(resp.event)->values) = t3;

    sendMessage(resp);
}

void CarduinoNode::handleTimeSyncResponse(uint32_t t2Ms, uint32_t t3Ms) {
    if (!_syncPending.exchange(false, std::memory_order_relaxed)) {
        // Risposta arrivata dopo il timeout (o senza una request pendente):
        // scartiamo per non applicare un offset calcolato su un RTT non più
        // valido/misurato.
        NLOGW("Time sync: TIME_SYNC_RESPONSE ricevuta senza request pendente, scartata");
        return;
    }

    uint32_t t4 = localMillis();
    uint32_t t1 = _syncT1Ms;

    // Schema NTP a 4 timestamp (single round, nessuna media):
    //   round_trip = (t4 - t1) - (t3 - t2)
    //   offset     = ((t2 - t1) + (t3 - t4)) / 2
    int32_t roundTrip = static_cast<int32_t>(t4 - t1) - static_cast<int32_t>(t3Ms - t2Ms);
    int32_t offset = (static_cast<int32_t>(t2Ms - t1) + static_cast<int32_t>(t3Ms - t4)) / 2;

    _timeOffsetMs = offset;
    _timeSynced.store(true, std::memory_order_relaxed);

    NLOGI("Time sync completato: RTT=%d ms, offset=%d ms", roundTrip, offset);
}

void CarduinoNode::enable() {
    NLOGI("CarduinoNode::enable called");
}

void CarduinoNode::disable() {
    NLOGI("CarduinoNode::disable called");
}

void CarduinoNode::enableInterrupt() {
    NLOGI("CarduinoNode::enableInterrupt called");
}

void CarduinoNode::disableInterrupt() {
    NLOGI("CarduinoNode::disableInterrupt called");
}

void CarduinoNode::restart() {
    NLOGI("CarduinoNode::restart called");
}

void CarduinoNode::heartbeatReceived() {
    NLOGI("CarduinoNode::heartbeatReceived called");
}

void CarduinoNode::test() {
    NLOGI("CarduinoNode::test called");
}