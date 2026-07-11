#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "esp_netif.h"
#include "nvs_flash.h"

#include "NodeLog.h"
#include "Definitions.h"

// ---------------------------------------------------------------------------
// Configurazione (Kconfig ha priorità; questi sono fallback se non si usa
// menuconfig / non sono stati definiti).
// ---------------------------------------------------------------------------

// #ifdef CONFIG_UDP_LOG_WIFI_SSID
//     #define UDP_LOG_WIFI_SSID       CONFIG_UDP_LOG_WIFI_SSID
// #elif !defined(UDP_LOG_WIFI_SSID)
//     #define UDP_LOG_WIFI_SSID       "your-ssid"
// #endif

// #ifdef CONFIG_UDP_LOG_WIFI_PASSWORD
//     #define UDP_LOG_WIFI_PASSWORD   CONFIG_UDP_LOG_WIFI_PASSWORD
// #elif !defined(UDP_LOG_WIFI_PASSWORD)
//     #define UDP_LOG_WIFI_PASSWORD   "your-password"
// #endif

// #ifdef CONFIG_UDP_LOG_PORT
//     #define UDP_LOG_PORT            CONFIG_UDP_LOG_PORT
// #elif !defined(UDP_LOG_PORT)
//     #define UDP_LOG_PORT            3333
// #endif

// Dimensione massima di una singola riga di log formattata.
#ifndef UDP_LOG_BUF_SIZE
    #define UDP_LOG_BUF_SIZE        256
#endif

// Numero massimo di tentativi di riconnessione WiFi "veloci" prima di
// passare a un backoff più lungo (puramente informativo/log interno,
// la riconnessione continua comunque all'infinito in background).
#ifndef UDP_LOG_WIFI_MAX_RETRY
    #define UDP_LOG_WIFI_MAX_RETRY  5
#endif

#ifndef SERIAL_LOG_DEFAULT
#define SERIAL_LOG_DEFAULT 1  // fallback se non definito in platformio.ini
#endif

/**
 * UdpLogSender
 *
 * Componente di sistema (non legato a un singolo nodo CAN) che:
 *  - si connette in WiFi come STA usando SSID/password configurabili
 *  - intercetta tutti gli ESP_LOG* tramite esp_log_set_vprintf
 *  - continua a stampare normalmente su console
 *  - inoltre, se abilitato e se il WiFi è connesso, inoltra la stessa riga
 *    come UDP broadcast (255.255.255.255) sulla subnet locale
 *  - gestisce la riconnessione WiFi in background senza bloccare il resto
 *    del sistema
 *
 * Pensata per essere una classe base: CarduinoNode (e altre classi) la
 * estendono per ottenere automaticamente il logging via UDP, abilitabile
 * e disabilitabile a runtime con enableUdpLog()/disableUdpLog().
 *
 * Nota: essendo un servizio di sistema unico (un solo WiFi, una sola
 * socket UDP condivisa), tutto lo stato interno è statico/globale alla
 * classe; istanziare più oggetti derivati non crea più connessioni WiFi
 * o più socket.
 */
class UdpLogSender {
public:
    UdpLogSender();

    void udp_log_sender_init();

    // Abilita/disabilita l'inoltro UDP a runtime. La stampa su console
    // (comportamento standard di ESP_LOG*) non è mai influenzata da questo
    // flag: resta sempre attiva.
    void enableUdpLog();
    void disableUdpLog();
    bool isUdpLogEnabled() const;

    // True se il WiFi STA è attualmente connesso e ha un IP valido.
    bool isWifiConnected() const;

    static void setSerialEnabled(bool enabled) { s_serialEnabled = enabled; }
    static bool isSerialEnabled() { return s_serialEnabled; }

protected:
    bool s_initialized;
    volatile bool s_udpEnabled;
    volatile bool s_wifiConnected;
    int s_sockfd;
    vprintf_like_t s_originalVprintf;

private:
    static void wifiEventHandlerTrampoline(void *arg, esp_event_base_t event_base, 
                                    int32_t event_id, void *event_data);

    void handleWifiEvent(esp_event_base_t event_base, int32_t event_id, void *event_data);

    static int vprintfHookTrampoline(const char* fmt, va_list args);

    int vprintfHook(const char* fmt, va_list args);

    void sendUdpLine(const char* data, int len);

    // Buffer statico riusato per ogni riga di log: niente allocazioni nel
    // hot path. Non c'è protezione concorrente esplicita: i log provenienti
    // da task diversi sono già serializzati dal mutex interno della libreria
    // di log di ESP-IDF, quindi il vprintf hook non viene mai eseguito in
    // modo concorrente da due thread sullo stesso buffer.
    char s_lineBuf[UDP_LOG_BUF_SIZE];
    
    static bool s_serialEnabled;
};