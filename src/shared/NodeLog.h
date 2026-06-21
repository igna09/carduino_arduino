#pragma once

#include "esp_log.h"

/*
 * NodeLog.h
 *
 * Wrapper attorno a ESP_LOGx che usa automaticamente NODE_NAME come tag.
 * NODE_NAME deve essere definito a compile-time PRIMA di includere questo
 * header, es:
 *
 *   #define NODE_NAME "KLINE"
 *   #include "NodeLog.h"
 *
 * Tipicamente lo metti come build flag per-componente (platformio.ini /
 * CMakeLists.txt) oppure come primo #define nel .cpp del nodo.
 *
 * ---------------------------------------------------------------------
 * Integrazione con UdpLogSender
 * ---------------------------------------------------------------------
 * Se UdpLogSender è agganciato via esp_log_set_vprintf(), NON serve fare
 * nulla qui: ESP_LOGx (quindi anche NLOGx) passa già automaticamente
 * anche per UDP, perché è esp_log stesso a richiamare il tuo vprintf hook
 * per ogni riga di log, indipendentemente dal tag.
 *
 * Se invece UdpLogSender espone un metodo esplicito (es. send(tag, msg))
 * che vuoi invocare IN AGGIUNTA a ESP_LOGx (quindi non tramite hook),
 * usa la variante in fondo al file (sezione "VARIANTE A CHIAMATA
 * ESPLICITA"), commentando la sezione di default.
 */

#ifndef NODE_NAME
#error "NODE_NAME non definito: fare #define NODE_NAME \"...\" prima di includere NodeLog.h"
#endif

// ---------------------------------------------------------------------
// VARIANTE DI DEFAULT: passthrough puro su ESP_LOGx (per uso con hook
// esp_log_set_vprintf già configurato in UdpLogSender, es. nel costruttore
// o in un init() chiamato una volta all'avvio)
// ---------------------------------------------------------------------

#define NLOGE(format, ...) ESP_LOGE(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGW(format, ...) ESP_LOGW(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGI(format, ...) ESP_LOGI(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGD(format, ...) ESP_LOGD(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGV(format, ...) ESP_LOGV(NODE_NAME, format, ##__VA_ARGS__)

// Varianti "_EARLY" se ti serve loggare prima che il log subsystem sia
// completamente inizializzato (rare, ma utili in fase di boot/ISR-safe)
#define NLOGE_EARLY(format, ...) ESP_EARLY_LOGE(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGW_EARLY(format, ...) ESP_EARLY_LOGW(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGI_EARLY(format, ...) ESP_EARLY_LOGI(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGD_EARLY(format, ...) ESP_EARLY_LOGD(NODE_NAME, format, ##__VA_ARGS__)
#define NLOGV_EARLY(format, ...) ESP_EARLY_LOGV(NODE_NAME, format, ##__VA_ARGS__)

// ---------------------------------------------------------------------
// VARIANTE A CHIAMATA ESPLICITA (decommenta se UdpLogSender NON è
// agganciato via vprintf hook, e devi chiamarlo a parte)
// ---------------------------------------------------------------------
//
// Richiede che esista un'istanza globale/raggiungibile, es.
// extern UdpLogSender g_udpLogSender;
// e un metodo del tipo: void send(const char* tag, esp_log_level_t level,
//                                  const char* fmt, va_list args);
//
// #define NLOGE(format, ...)                                            
//     do {                                                               
//         ESP_LOGE(NODE_NAME, format, ##__VA_ARGS__);                    
//         g_udpLogSender.sendf(NODE_NAME, ESP_LOG_ERROR, format, ##__VA_ARGS__); 
//     } while (0)
//
// (ripetere per W/I/D/V cambiando solo il livello)