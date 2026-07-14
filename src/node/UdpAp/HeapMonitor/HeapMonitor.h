#pragma once

// Task diagnostico: stampa periodicamente lo stato dell'heap su seriale/USB.
// Utile per capire quanto margine c'e' prima di allargare buffer statici
// (es. WEB_LOG_RING_SIZE) senza rischiare fallimenti di allocazione a runtime.
namespace HeapMonitor {

// intervalMs: periodo di stampa (es. 5000 per ogni 5 secondi)
void start(uint32_t intervalMs = 5000);

} // namespace HeapMonitor