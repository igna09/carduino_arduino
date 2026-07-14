#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"

#include "HeapMonitor.h"
#include "NodeLog.h"

namespace HeapMonitor {

// Stack basso: il task fa solo query heap + printf, non serve piu' di 3072 byte
static constexpr uint32_t TASK_STACK_SIZE = 3072;
static constexpr UBaseType_t TASK_PRIORITY = 1; // priorita' bassa: e' solo diagnostica

struct TaskArgs {
    uint32_t intervalMs;
};

static void monitorTask(void* pvParams) {
    TaskArgs* args = static_cast<TaskArgs*>(pvParams);
    const TickType_t delayTicks = pdMS_TO_TICKS(args->intervalMs > 0 ? args->intervalMs : 1);

    // Stampiamo subito la baseline appena il task parte
    NLOGI("[HeapMonitor] Avviato (intervallo %lu ms)", (unsigned long)args->intervalMs);

    while (1) {
        // Heap generico (8-bit capable), quello rilevante per allocazioni std::/new/malloc
        size_t freeHeap      = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        size_t minEverFree    = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
        size_t largestBlock   = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);

        // Stack watermark del task chiamante corrente (utile anche per altri task se lo si copia)
        UBaseType_t stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);

        // Stampa su USB/seriale via printf (che finisce sul canale di log corrente,
        // tipicamente USB CDC su ESP32-C3). Formato compatto per non affollare il monitor.
        printf("[HeapMonitor] free=%u min_ever=%u largest_block=%u self_stack_hwm=%u\n",
               (unsigned)freeHeap,
               (unsigned)minEverFree,
               (unsigned)largestBlock,
               (unsigned)stackHighWaterMark);

        vTaskDelay(delayTicks);
    }
}

void start(uint32_t intervalMs) {
    // TaskArgs statico: il task vive per tutta la durata del programma,
    // non serve gestirne la deallocazione
    static TaskArgs args;
    args.intervalMs = intervalMs;

    xTaskCreate(monitorTask, "heap_monitor", TASK_STACK_SIZE, &args, TASK_PRIORITY, nullptr);
}

} // namespace HeapMonitor