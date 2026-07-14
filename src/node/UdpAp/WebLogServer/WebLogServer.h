#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// WebLogServer
//
// Piccolo server HTTP (esp_http_server) che espone:
//   GET /         -> pagina HTML "terminale" con filtro client-side
//   GET /events   -> Server-Sent Events, stream live delle righe UDP
//
// Mantiene un ring buffer indipendente da quello della console seriale
// (s_filterNodes in UdpAp.cpp): qui non c'e' alcun filtro server-side,
// ogni riga ricevuta via UDP viene pubblicata sia al log seriale che a
// questo modulo, e il filtro lo applica il browser via JS.
//
// Pensato per essere "fire and forget": init() avvia il task HTTP,
// pushLine() va chiamato dal punto in cui udp_server_task in UdpAp.cpp
// gia' stampa la riga con NLOGI, cosi' i due percorsi restano disaccoppiati.
// ---------------------------------------------------------------------------
namespace WebLogServer {

// Avvia il server HTTP. Va chiamato dopo che la rete (SoftAP) e' attiva,
// tipicamente subito dopo wifi_init_softap() in app_main().
void init();

// Pubblica una riga di log verso i client web connessi (SSE) e nel ring
// buffer di history. Troncata internamente a WEB_LOG_LINE_MAX-1 caratteri.
// Thread-safe: usa un mutex dedicato, separato da quello della console.
void pushLine(const char* line);

// Varianti comode che compongono la riga "[prefix]: msg" internamente,
// in un buffer gia' dimensionato a WEB_LOG_LINE_MAX. Usarle al posto di
// formattare in un buffer separato in UdpAp.cpp evita doppie troncature
// e i relativi warning -Wformat-truncation del compilatore, dato che qui
// la dimensione di destinazione e' nota staticamente.
void pushLineFormatted(const char* prefix, const char* msg);

// static void setFilter(const char* text);

} // namespace WebLogServer