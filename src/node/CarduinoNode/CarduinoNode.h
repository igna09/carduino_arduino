#pragma once

#include <stdio.h>
#include <string>
#include <functional>
#include <map>
#include <atomic>
#include "esp_twai.h"
#include "esp_twai_onchip.h"

#include "SettingBase.h"
#include "Message.h"
#include "Node.h"
#include "Definitions.h"
#include "UdpLogSender.h"
#include "NodeLog.h"
#include "Executor.h"
#include "CarduinoNodeSerialWriteSetting.h"

#define TWAI_QUEUE_DEPTH        10
#define TWAI_BITRATE            1000000

// --- Configurazione RX pool / recovery, per-nodo ---
#define CAN_RX_POOL_DEPTH        32      // profondità pool di ricezione per-nodo
#define CAN_RECOVERY_WAIT_MS     1000    // attesa massima dopo twai_node_recover()
#define CAN_RECOVERY_STEP_MS     50
#define CAN_RECOVERY_SAFETY_MS   500     // wake periodico di sicurezza del recovery task

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
    bool isEnabled;
    Executor _serialExecutor;
    Executor _canExecutor;

    CarduinoNode(uint8_t id);
    std::string name();
    void sendMessage(const Message& m);
    void sendByte(uint16_t messageId, int len, uint8_t *buf);
    void delayTask(unsigned long millisec, std::function<void()> lambda);
    void startRepeatingTask(const std::string& id, unsigned long millisec, std::function<void()> fn, uint32_t stackSize = 4096, UBaseType_t priority = 5);
    void stopRepeatingTask(const std::string& id);
    void stopAllRepeatingTasks();

    // Hook di dispatch per i messaggi ricevuti dal bus CAN.
    // Settabile dall'esterno (es. dal main) per collegare il routing applicativo
    // senza che CarduinoNode debba conoscere la logica a valle.
    // Il Message* è owning: chi riceve la callback ne diventa responsabile
    // (deve fare delete quando ha finito).
    void onMessageReceived(std::function<void(Message*)> handler);

private:
    uint8_t _id;
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

    // Callback applicativa per i messaggi ricevuti (default: nessuna azione)
    std::function<void(Message*)> _onMessage = nullptr;

    // --- Setup interno ---
    void setupRxPool();
    void registerTwaiCallbacks();
    void startRecoveryTask();
    void startRxTask();

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