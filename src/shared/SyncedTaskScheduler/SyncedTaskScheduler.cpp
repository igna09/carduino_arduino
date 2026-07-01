#include "SyncedTaskScheduler.h"
#include "CarduinoNode.h"
#include "NodeLog.h"

#include <vector>

SyncedTaskScheduler::SyncedTaskScheduler(CarduinoNode* node) : _node(node) {
    _mutex = xSemaphoreCreateMutex();
    assert(_mutex != nullptr);
}

SyncedTaskScheduler::~SyncedTaskScheduler() {
    clear();
    vSemaphoreDelete(_mutex);
}

uint32_t SyncedTaskScheduler::boundaryAtOrBefore(uint32_t now, uint32_t periodMs, uint32_t phaseMs) {
    int64_t delta = static_cast<int64_t>(now) - static_cast<int64_t>(phaseMs);
    if (delta < 0) {
        return UINT32_MAX; // fase non ancora raggiunta, nessun boundary valido
    }
    uint32_t k = static_cast<uint32_t>(delta / periodMs);
    return phaseMs + k * periodMs;
}

uint32_t SyncedTaskScheduler::nextBoundaryAfter(uint32_t now, uint32_t periodMs, uint32_t phaseMs) {
    uint32_t b = boundaryAtOrBefore(now, periodMs, phaseMs);
    if (b == UINT32_MAX) {
        return phaseMs; // il primo boundary in assoluto è la fase stessa
    }
    return b + periodMs;
}

// Entry-point del task dedicato a UNA singola lambda registrata.
void SyncedTaskScheduler::taskTrampoline(void* pv) {
    auto* e = static_cast<Entry*>(pv);

    // Target esplicito del prossimo boundary da eseguire, tracciato come
    // variabile di stato (non ricalcolato da zero ogni giro tramite
    // boundaryAtOrBefore/now): questo è ciò che evita la cascata di doppie
    // esecuzioni osservata quando il task viene schedulato in ritardo
    // (starvation, lambda precedente lenta, notify spurie, tick rounding).
    //
    // Un target esplicito garantisce che, qualunque cosa succeda tra un
    // risveglio e l'altro, ogni boundary venga eseguito AL MASSIMO una
    // volta, e che il prossimo target sia sempre calcolato rispetto al
    // tempo REALE dopo l'esecuzione, non rispetto a un vecchio target
    // "inseguito" a raffica.
    uint32_t target = nextBoundaryAfter(e->node->syncedMillis(), e->periodMs, e->phaseMs);

    while (!e->stop) {
        uint32_t now = e->node->syncedMillis();
        uint32_t waitMs = (target > now) ? (target - now) : 0;

        // Sleep interrompibile: removeTask()/clear() fanno una notify per
        // svegliare subito il task invece di aspettare il prossimo boundary
        // quando si vuole fermarlo. NOTA: questo risveglio non implica che
        // il target sia stato raggiunto (potrebbe essere una notify spuria
        // o un risveglio anticipato per arrotondamento dei tick FreeRTOS,
        // tipicamente 10ms) — è per questo che sotto ricontrolliamo 'now'
        // esplicitamente invece di fidarci del semplice fatto di esserci
        // svegliati.
        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(waitMs));
        if (e->stop) break;

        now = e->node->syncedMillis();
        if (now < target) {
            // Risveglio anticipato (arrotondamento tick o notify spuria):
            // il target non è stato raggiunto, si ridorme per il tempo
            // residuo. 'target' NON viene toccato: lo stesso identico
            // boundary resta da eseguire, non ne "perdiamo" né ne
            // duplichiamo nessuno.
            continue;
        }

        // Target raggiunto (o superato, se il task è stato in ritardo):
        // eseguiamo ESATTAMENTE UNA VOLTA per questo target, quale che sia
        // stato il ritardo accumulato.
        //
        // La lambda gira nel SUO task dedicato: può bloccare con vTaskDelay
        // (o qualunque altra primitiva) senza impattare altre entry
        // registrate su questo stesso SyncedTaskScheduler, ognuna ha il
        // proprio task indipendente.
        e->fn();

        // Riallineamento: il prossimo target si calcola SEMPRE rispetto al
        // tempo REALE dopo l'esecuzione di fn() (che potrebbe aver
        // bloccato per un po'), non incrementando ciecamente 'target' di
        // periodMs. Se fn() ha impiegato più di un periodo, questo salta
        // correttamente in avanti al prossimo boundary futuro valido senza
        // tentare di "recuperare" i boundary persi con esecuzioni multiple
        // ravvicinate.
        target = nextBoundaryAfter(e->node->syncedMillis(), e->periodMs, e->phaseMs);
    }

    // Segnala a chi ha chiamato removeTask() (se in attesa) che questo task
    // sta per terminare davvero, PRIMA di eliminare la entry: da questo
    // punto in poi è garantito che fn() non verrà più chiamata da questo
    // task.
    if (e->doneSem != nullptr) {
        xSemaphoreGive(e->doneSem);
    }

    delete e;
    vTaskDelete(NULL);
}

void SyncedTaskScheduler::addTask(const std::string& id, uint32_t periodMs, std::function<void()> fn,
                                   uint32_t phaseMs, uint32_t stackSize, UBaseType_t priority) {
    if (periodMs == 0) {
        NLOGW("SyncedTaskScheduler::addTask: periodMs=0 per id=%s, ignorato", id.c_str());
        return;
    }

    // Se esiste già una entry con questo id, ferma il vecchio task prima di
    // crearne uno nuovo (sostituzione, stesso pattern di startRepeatingTask
    // in CarduinoNode).
    removeTask(id);

    auto* e = new Entry();
    e->id = id;
    e->periodMs = periodMs;
    e->phaseMs = phaseMs;
    e->fn = std::move(fn);
    e->node = _node;

    // Nome task troncato per restare dentro i limiti FreeRTOS
    // (configMAX_TASK_NAME_LEN tipicamente 16, terminatore incluso).
    char taskName[16];
    snprintf(taskName, sizeof(taskName), "st_%.12s", id.c_str());

    xTaskCreate(SyncedTaskScheduler::taskTrampoline, taskName, stackSize, e, priority, &e->taskHdl);

    xSemaphoreTake(_mutex, portMAX_DELAY);
    _entries[id] = e;
    xSemaphoreGive(_mutex);
}

void SyncedTaskScheduler::removeTask(const std::string& id) {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    auto it = _entries.find(id);
    if (it == _entries.end()) {
        xSemaphoreGive(_mutex);
        return;
    }
    Entry* e = it->second;
    _entries.erase(it);
    xSemaphoreGive(_mutex);

    // Creiamo il semaforo di conferma PRIMA di segnalare lo stop, altrimenti
    // ci sarebbe una finestra in cui il task potrebbe già essere arrivato in
    // fondo al loop e provare a darlo quando ancora non esiste.
    SemaphoreHandle_t doneSem = xSemaphoreCreateBinary();
    e->doneSem = doneSem;

    // Segnala lo stop e sveglia subito il task (se sta dormendo su
    // ulTaskNotifyTake in attesa del prossimo boundary).
    e->stop = true;
    if (e->taskHdl != nullptr) {
        xTaskNotifyGive(e->taskHdl);
    }
    // Da qui in poi NON tocchiamo più 'e': appena il task dà doneSem, esegue
    // subito "delete e" (vedi taskTrampoline), quindi il puntatore può
    // diventare invalido in qualunque momento dopo la xTaskNotifyGive sopra.
    // Usiamo solo la copia locale 'doneSem' da qui in avanti.

    // Attende la conferma di terminazione effettiva. Se il task era già a
    // metà di fn() quando abbiamo settato stop, questa fn() in corso viene
    // comunque lasciata finire (cancellazione cooperativa, non preemptive:
    // vedi motivazione sotto), ma removeTask() NON ritorna finché non è
    // garantito che nessuna nuova invocazione di fn() possa più avvenire.
    // Questo chiude la finestra di race che permetteva a un vecchio task
    // "in coda alla sua vita" e un nuovo task appena creato con lo stesso id
    // di sovrapporsi ed eseguire entrambi vicino allo stesso boundary.
    //
    // Timeout di sicurezza: se fn() dovesse bloccare indefinitamente (bug
    // della lambda registrata, es. un vTaskDelay(portMAX_DELAY) o un mutex
    // mai rilasciato altrove), non vogliamo restare bloccati per sempre qui
    // dentro; logghiamo un warning e procediamo comunque (il vecchio task
    // resterà "orfano" finché non sblocca da solo, ma non impedisce al
    // resto del sistema di proseguire).
    if (xSemaphoreTake(doneSem, pdMS_TO_TICKS(5000)) != pdTRUE) {
        NLOGW("SyncedTaskScheduler::removeTask: timeout attesa terminazione task id=%s (fn() bloccata?)", id.c_str());
    }
    vSemaphoreDelete(doneSem);
    // NB: 'e' viene comunque deallocata dal task stesso (delete e; dentro
    // taskTrampoline) subito dopo aver dato doneSem: non tocchiamo più 'e'
    // da qui in poi, il puntatore potrebbe già essere invalido.
}

void SyncedTaskScheduler::clear() {
    xSemaphoreTake(_mutex, portMAX_DELAY);
    std::vector<Entry*> toStop;
    toStop.reserve(_entries.size());
    for (auto& [id, e] : _entries) {
        toStop.push_back(e);
    }
    _entries.clear();
    xSemaphoreGive(_mutex);

    for (auto* e : toStop) {
        e->stop = true;
        if (e->taskHdl != nullptr) {
            xTaskNotifyGive(e->taskHdl);
        }
    }
}