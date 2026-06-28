#pragma once

#include <stdio.h>
#include <string>
#include <functional>
#include <map>
#include <atomic>
#include <iostream>
#include "esp_twai.h"
#include "esp_twai_onchip.h"
#include "esp_timer.h"

#include "SettingBase.h"
#include "Message.h"
#include "Node.h"
#include "Definitions.h"
#include "UdpLogSender.h"
#include "NodeLog.h"
#include "Executor.h"
#include "CarduinoNodeSerialWriteSetting.h"
#include "EventMulti.h"
#include "CarduinoNodeCanEvent.h"

#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000

// --- Configurazione RX pool / recovery, per-nodo ---
#define CAN_RX_POOL_DEPTH        32      // profondità pool di ricezione per-nodo
#define CAN_RECOVERY_WAIT_MS     1000    // attesa massima dopo twai_node_recover()
#define CAN_RECOVERY_STEP_MS     50
#define CAN_RECOVERY_SAFETY_MS   500     // wake periodico di sicurezza del recovery task

// --- Time sync (ping-pong stile NTP, single-shot all'avvio/enable) ---------
#define TIME_SYNC_TIMEOUT_MS     2000    // se non arriva la response entro questo tempo, sync considerato fallito (loggato una volta)

// --- Hello (annuncio periodico finché il nodo non viene enablato) ---------
#define HELLO_TASK_ID            "hello_task"
#define HELLO_PERIOD_MS          1000    // intervallo tra un HELLO e il successivo, mentre isEnabled == false

struct RepeatingTaskCtx {
    std::function<void()> fn;
    uint32_t periodMs;
    std::string id;
    volatile bool stop;
};
struct TaskEntry {
    TaskHandle_t handle;
    RepeatingTaskCtx* ctx;
};

// Singolo slot del pool di ricezione: frame + buffer dati di backing
// (twai_frame_t.buffer è un puntatore, deve puntare a memoria stabile per
// tutta la vita dello slot, quindi la teniamo qui dentro).
struct CanRxSlot {
    twai_frame_t frame;
    uint8_t data[TWAI_FRAME_MAX_LEN];
};

class CarduinoNode: public SettingBase, public UdpLogSender {
public:
    uint8_t _id;
    // True solo dopo una chiamata a enable() andata a buon fine (richiamare
    // enable() quando isEnabled è già true non ha alcun effetto, vedi cpp).
    // Finché resta false, il nodo manda HELLO periodicamente (vedi
    // HELLO_TASK_ID); MainNode parte già enabled di default e quindi non
    // manda mai HELLO.
    bool isEnabled;
    Executor _serialExecutor;
    Executor _canExecutor;

    CarduinoNode(uint8_t id);
    
    std::string name();
    void sendMessage(const Message& m);
    void sendSerialMessage(const Message& m);
    void sendByte(uint16_t messageId, int len, uint8_t *buf);
    void delayTask(unsigned long millisec, std::function<void()> lambda);
    void startRepeatingTask(const std::string& id, unsigned long millisec, std::function<void()> fn, uint32_t stackSize = 4096, UBaseType_t priority = 5);
    void stopRepeatingTask(const std::string& id);
    void stopAllRepeatingTasks();
    virtual void enable();
    void disable();
    void enableInterrupt();
    void disableInterrupt();
    void restart();
    void heartbeatReceived();
    void test();

    // -------------------------------------------------------------------
    // Time sync
    //   syncedMillis() ritorna il "tempo di rete" stimato: il clock locale
    //   (esp_timer_get_time()/1000) corretto con l'offset calcolato durante
    //   il sync ping-pong con MAIN. Su MAIN stesso l'offset è 0 by design
    //   (MainNode fa override per tornare valori fissi, vedi sotto).
    //
    //   ATTENZIONE: se il sync non è (ancora) avvenuto, syncedMillis() NON
    //   blocca né solleva errori: fa fallback silenzioso al clock locale
    //   (equivalente a offset=0), ma loggando un warning una sola volta.
    //   Chi ha bisogno di sapere se il valore è affidabile DEVE controllare
    //   isTimeSynced() prima di usare il risultato per correlare eventi tra
    //   nodi diversi.
    // -------------------------------------------------------------------
    virtual bool isTimeSynced() const;
    virtual uint32_t syncedMillis() const;

    // Avvia il sync ping-pong verso MAIN (manda TIME_SYNC_REQUEST).
    // Chiamato internamente quando il nodo riceve EV_ENABLE in seguito al
    // proprio annuncio EV_HELLO, ma esposto anche pubblicamente per un
    // eventuale retry manuale.
    void startTimeSync();

    // Gestori chiamati da CarduinoNodeCanEvent in risposta ai relativi
    // eventi CAN. Pubblici perché invocati dall'executor, non da node stesso.
    void handleTimeSyncRequest(uint8_t requesterId);
    void handleTimeSyncResponse(uint32_t t2Ms, uint32_t t3Ms);

private:
    twai_node_handle_t _twai_node = NULL;
    std::map<std::string, TaskEntry> tasks_;

    // --- Stato bus / recovery (per-nodo) ---
    std::atomic<bool> _busOff{false};
    std::atomic<bool> _recoveryInProgress{false};
    TaskHandle_t       _recoveryTaskHdl = nullptr;

    // --- RX pool (per-nodo) ---
    CanRxSlot*         _rxPool = nullptr;
    SemaphoreHandle_t  _freePoolSemaphore = nullptr;
    SemaphoreHandle_t  _rxResultSemaphore = nullptr;
    int                _rxWriteIdx = 0;
    int                _rxReadIdx = 0;
    TaskHandle_t       _rxTaskHdl = nullptr;

    // --- Time sync (per-nodo) ---
    std::atomic<bool>    _timeSynced{false};
    int32_t              _timeOffsetMs = 0;       // syncedMillis() = localMillis() + offset
    uint32_t             _syncT1Ms = 0;            // timestamp locale di invio TIME_SYNC_REQUEST, in attesa di risposta
    std::atomic<bool>    _syncPending{false};      // true tra l'invio della request e la response (o il timeout)
    bool                 _unsyncedWarnLogged = false; // evita spam di log se syncedMillis() viene chiamato pre-sync

    static uint32_t localMillis() { return static_cast<uint32_t>(esp_timer_get_time() / 1000); }

    // --- Setup interno ---
    void setupRxPool();
    void registerTwaiCallbacks();
    void startRecoveryTask();
    void startRxTask();

    // Costruisce e invia il messaggio EV_HELLO (id del nodo nel payload),
    // destinato a Node::MAIN. Richiamato dal repeating task avviato nel
    // costruttore finché isEnabled è false.
    void sendHello();

    // --- Callback ISR statici (firma richiesta dal driver TWAI) ---
    static bool IRAM_ATTR onRxDoneCallback(twai_node_handle_t handle,
                                            const twai_rx_done_event_data_t *edata,
                                            void *user_ctx);
    static bool IRAM_ATTR onStateChangeCallback(twai_node_handle_t handle,
                                                 const twai_state_change_event_data_t *edata,
                                                 void *user_ctx);
    static bool IRAM_ATTR onErrorCallback(twai_node_handle_t handle,
                                           const twai_error_event_data_t *edata,
                                           void *user_ctx);

    // --- Task entry-point statici (FreeRTOS richiede puntatori a funzione liberi) ---
    static void recoveryTaskEntry(void *pvParameters);
    static void rxTaskEntry(void *pvParameters);
};