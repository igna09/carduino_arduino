#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <map>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

class CarduinoNode;

// ============================================================================
// SyncedTaskScheduler
//
//   Esegue lambda diverse, con periodi (ed eventuale fase) diversi, allineate
//   ai multipli "tondi" del tempo di rete (CarduinoNode::syncedMillis()).
//   Usando lo stesso periodo/fase su nodi diversi già sincronizzati tra loro
//   (vedi CarduinoNode::isTimeSynced()), le rispettive lambda scattano nello
//   stesso istante di rete su tutti i nodi coinvolti, indipendentemente da
//   quando ciascun nodo ha chiamato addTask().
//
//   OGNI lambda registrata ha il proprio task FreeRTOS dedicato: può quindi
//   bloccare con vTaskDelay() (o qualunque altra primitiva bloccante) senza
//   impattare le altre lambda sincronizzate, ognuna gira per conto suo. Il
//   costo è uno stack FreeRTOS per task registrato: per molte decine di
//   lambda valutare se serva davvero un task a testa, ma per un numero
//   ragionevole (manciata di task per nodo, tipico in questo progetto) è il
//   compromesso più semplice e robusto.
//
//   ATTENZIONE: finché il nodo non ha completato il time sync (vedi
//   CarduinoNode::isTimeSynced()), syncedMillis() fa fallback silenzioso sul
//   clock locale: i boundary restano deterministici e coerenti SU QUESTO
//   nodo, ma non saranno allineati a quelli di altri nodi finché il sync non
//   converge.
// ============================================================================
class SyncedTaskScheduler {
public:
    // node: il nodo da cui leggere syncedMillis(). Non viene posseduto né
    // distrutto da questa classe.
    explicit SyncedTaskScheduler(CarduinoNode* node);
    ~SyncedTaskScheduler();

    // Crea un task FreeRTOS dedicato che esegue fn() ogni volta che
    // syncedMillis() raggiunge un multiplo di periodMs (eventualmente
    // shiftato di phaseMs). Se esiste già una entry con lo stesso id, il
    // vecchio task viene fermato e sostituito da questo nuovo.
    //
    // Esempi:
    //   addTask("blink", 1000, fn)            -> scatta a 1000, 2000, 3000, ...
    //   addTask("report", 5000, fn, 200)      -> scatta a 200, 5200, 10200, ...
    //
    // fn può bloccare liberamente (vTaskDelay, semafori, ecc.): ha un task
    // tutto suo, non condiviso con altre entry registrate qui.
    //
    // id: identificatore univoco per poter rimuovere/sostituire la entry.
    // periodMs: deve essere > 0.
    // phaseMs: offset rispetto ai multipli "tondi" (default 0).
    // stackSize/priority: passati a xTaskCreate per il task dedicato.
    void addTask(const std::string& id, uint32_t periodMs, std::function<void()> fn,
                 uint32_t phaseMs = 0, uint32_t stackSize = 4096, UBaseType_t priority = 5);

    // Ferma e rimuove il task dedicato per questo id. No-op se non esiste.
    // Il task si autodistrugge (delete + vTaskDelete) non appena si sveglia
    // dalla sleep corrente: la chiamata segnala lo stop e ritorna subito,
    // senza attendere la distruzione effettiva (stesso pattern già usato in
    // CarduinoNode::stopRepeatingTask).
    void removeTask(const std::string& id);

    // Ferma e rimuove tutti i task dedicati registrati.
    void clear();

private:
    struct Entry {
        std::string id;
        uint32_t periodMs;
        uint32_t phaseMs;
        std::function<void()> fn;
        CarduinoNode* node = nullptr;
        TaskHandle_t taskHdl = nullptr;
        volatile bool stop = false;
        // Dato in taskTrampoline appena prima di vTaskDelete(NULL): serve a
        // removeTask() per attendere la terminazione EFFETTIVA del task
        // prima di ritornare, evitando la finestra in cui il vecchio task
        // potrebbe essere ancora dentro fn() mentre un nuovo addTask() con
        // lo stesso id ha già creato ed avviato il task sostitutivo (visto
        // in pratica: esecuzioni multiple ravvicinate/nello stesso ms).
        SemaphoreHandle_t doneSem = nullptr;
    };

    CarduinoNode* _node;
    // Entry* anziché Entry per valore: l'ownership passa al task dedicato,
    // che si occupa lui stesso di "delete this" quando termina (vedi
    // taskTrampoline). La mappa qui serve solo per trovare/fermare le entry
    // per id, non per la loro distruzione.
    std::map<std::string, Entry*> _entries;
    SemaphoreHandle_t _mutex;

    static void taskTrampoline(void* pv);

    // Boundary più recente <= now per la entry data, oppure UINT32_MAX se
    // la fase non è ancora stata raggiunta.
    static uint32_t boundaryAtOrBefore(uint32_t now, uint32_t periodMs, uint32_t phaseMs);

    // Primo boundary strettamente successivo a now (usato per calcolare
    // quanto dormire prima della prossima esecuzione).
    static uint32_t nextBoundaryAfter(uint32_t now, uint32_t periodMs, uint32_t phaseMs);
};