#include "UdpLogSender.h"

// Puntatore statico locale al file (file-scoped) per aggirare la mancanza del parametro 'arg' nell'API C
static UdpLogSender* s_instance = nullptr;

UdpLogSender::UdpLogSender() {
    NLOGD("UdpLogSender::UdpLogSender start");

    s_instance = this;

    s_initialized = false;
    s_udpEnabled = false;
    s_wifiConnected = false;
    s_sockfd = -1;
    s_originalVprintf = nullptr;

    esp_log_level_set("wifi", ESP_LOG_WARN);

    this->udp_log_sender_init();
    
    NLOGD("UdpLogSender::UdpLogSender end");
}

void UdpLogSender::enableUdpLog() {
    NLOGD("UdpLogSender::enableUdpLog start");
    s_udpEnabled = true;
    NLOGD("UdpLogSender::enableUdpLog end");
}

void UdpLogSender::disableUdpLog() {
    s_udpEnabled = false;
}

bool UdpLogSender::isUdpLogEnabled() const {
    return s_udpEnabled;
}

bool UdpLogSender::isWifiConnected() const {
    return s_wifiConnected;
}

// ---------------------------------------------------------------------------
// Inizializzazione
// ---------------------------------------------------------------------------
void UdpLogSender::udp_log_sender_init() {
    NLOGD("UdpLogSender::udp_log_sender_init start");

    if (s_initialized) {
        return;
    }
    // s_lineBuf[UDP_LOG_BUF_SIZE];

    // NVS è richiesto dal driver WiFi per memorizzare i parametri di
    // calibrazione radio. Se è già stato inizializzato altrove nel
    // firmware, esp_wifi_init si limiterà a fallire silenziosamente su
    // quella parte specifica: qui lo inizializziamo in modo defensivo.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        NLOGW("UdpLogSender::udp_log_sender_init error");
        nvs_flash_erase();
        nvs_flash_init();
    }
    NLOGD("UdpLogSender::udp_log_sender_init flash initialized");

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    NLOGD("UdpLogSender::udp_log_sender_init mode set");

    wifi_init_config_t wifiInitCfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifiInitCfg);

    NLOGD("UdpLogSender::udp_log_sender_init initialized");

    esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &UdpLogSender::wifiEventHandlerTrampoline, this, nullptr);
    esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &UdpLogSender::wifiEventHandlerTrampoline, this, nullptr);

    NLOGD("UdpLogSender::udp_log_sender_init event set");

    wifi_config_t wifiConfig = {};
    strncpy(reinterpret_cast<char*>(wifiConfig.sta.ssid),
            UDP_LOG_WIFI_SSID, sizeof(wifiConfig.sta.ssid) - 1);
    strncpy(reinterpret_cast<char*>(wifiConfig.sta.password),
            UDP_LOG_WIFI_PASSWORD, sizeof(wifiConfig.sta.password) - 1);
    wifiConfig.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;

    NLOGD("UdpLogSender::udp_log_sender_init config set");

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);
    esp_wifi_start();
    // esp_wifi_start() fa scattare WIFI_EVENT_STA_START, gestito
    // nell'handler sotto, che a sua volta lancia esp_wifi_connect().
    // Tutto il resto (connessione effettiva, eventuale riconnessione)
    // avviene in modo asincrono: questa funzione ritorna immediatamente
    // e non blocca mai, anche se il WiFi non è disponibile.

    NLOGD("UdpLogSender::udp_log_sender_init wifi started");

    // Agganciamo il nostro hook al sistema di log, conservando il vprintf
    // originale per continuare a stampare su console esattamente come prima.
    s_originalVprintf = esp_log_set_vprintf(&UdpLogSender::vprintfHookTrampoline);

    NLOGI("UdpLogSender inizializzato (UDP broadcast porta %d)", UDP_LOG_PORT);

    s_initialized = true;

    NLOGD("UdpLogSender::udp_log_sender_init end");
}

int UdpLogSender::vprintfHookTrampoline(const char *fmt, va_list args) {
    // Se l'istanza della classe esiste, salta dentro il metodo non statico
    if (s_instance != nullptr) {
        return s_instance->vprintfHook(fmt, args);
    }
    
    // Fallback di sicurezza: se la classe non è ancora pronta, usa il vprintf standard di sistema
    return vprintf(fmt, args);
}

void UdpLogSender::wifiEventHandlerTrampoline(void *arg, esp_event_base_t event_base, 
                                       int32_t event_id, void *event_data) {
    NLOGD("UdpLogSender::wifiEventHandlerTrampoline start");
    // Convertiamo il puntatore generico void* nel tipo della nostra classe
    auto* instance = static_cast<UdpLogSender*>(arg);
    
    if (instance != nullptr) {
        // Saltiamo dentro l'istanza della classe
        instance->handleWifiEvent(event_base, event_id, event_data);
    }
    NLOGD("UdpLogSender::wifiEventHandlerTrampoline end");
}

// ---------------------------------------------------------------------------
// Gestione eventi WiFi: connessione iniziale + riconnessione automatica.
// Eseguito nel contesto del task dell'event loop di default di ESP-IDF:
// nessun task dedicato necessario, nessun blocco del resto del sistema.
// ---------------------------------------------------------------------------
void UdpLogSender::handleWifiEvent(esp_event_base_t event_base, int32_t event_id, void* event_data) {
    NLOGD("UdpLogSender::handleWifiEvent start");
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        NLOGD("UdpLogSender::handleWifiEvent WIFI_EVENT_STA_START");
        esp_wifi_connect();
        return;
    }

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        NLOGD("UdpLogSender::handleWifiEvent WIFI_EVENT_STA_DISCONNECTED");
        s_wifiConnected = false;
        // Chiude la socket UDP: verrà ricreata alla prossima connessione
        // riuscita. Questo evita di tenere una socket "orfana" legata a
        // un'interfaccia che potrebbe cambiare IP/subnet alla riconnessione.
        if (s_sockfd >= 0) {
            close(s_sockfd);
            s_sockfd = -1;
        }
        NLOGD("WiFi disconnesso, riconnessione in corso...");
        // Riconnessione automatica, non bloccante: il driver WiFi gestisce
        // i ritardi/backoff internamente; qui ritentiamo semplicemente.
        esp_wifi_connect();
        return;
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        NLOGD("UdpLogSender::handleWifiEvent IP_EVENT_STA_GOT_IP");
        s_wifiConnected = true;

        // Crea (o ricrea) la socket UDP usata per il broadcast dei log.
        if (s_sockfd >= 0) {
            close(s_sockfd);
        }
        s_sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (s_sockfd >= 0) {
            int broadcastEnable = 1;
            setsockopt(s_sockfd, SOL_SOCKET, SO_BROADCAST,
                       &broadcastEnable, sizeof(broadcastEnable));
        } else {
            NLOGW("Creazione socket UDP per i log fallita");
        }

        NLOGI("WiFi connesso, log UDP %s",
                 s_udpEnabled ? "attivo" : "disponibile (disabilitato)");
        return;
    }
    NLOGD("UdpLogSender::handleWifiEvent end");
}

// ---------------------------------------------------------------------------
// Hook installato in esp_log_set_vprintf.
// ---------------------------------------------------------------------------
int UdpLogSender::vprintfHook(const char* fmt, va_list args) {
    // 1) Stampa su console esattamente come faceva prima del nostro hook.
    int ret;
    if (s_originalVprintf) {
        ret = s_originalVprintf(fmt, args);
    } else {
        va_list argsCopy;
        va_copy(argsCopy, args);
        ret = vprintf(fmt, argsCopy);
        va_end(argsCopy);
    }

    // 2) Se l'inoltro UDP non è richiesto o il WiFi non è connesso, ci
    //    fermiamo qui: il logging su console resta sempre funzionante,
    //    nessun blocco, nessun costo aggiuntivo.
    if (!s_udpEnabled || !s_wifiConnected || s_sockfd < 0) {
        return ret;
    }

    // 3) Formatta la riga nel buffer statico (nessuna allocazione dinamica).
    int len = vsnprintf(s_lineBuf, sizeof(s_lineBuf), fmt, args);
    if (len <= 0) {
        return ret;
    }
    if (len >= static_cast<int>(sizeof(s_lineBuf))) {
        len = sizeof(s_lineBuf) - 1;
    }

    sendUdpLine(s_lineBuf, len);

    return ret;
}

// ---------------------------------------------------------------------------
// Invio UDP broadcast, completamente non bloccante e "fail-safe": qualsiasi
// errore viene ignorato silenziosamente (a parte un best-effort di log
// interno che NON richiama a sua volta l'hook, per evitare ricorsione).
// ---------------------------------------------------------------------------
void UdpLogSender::sendUdpLine(const char* data, int len) {
    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST); // 255.255.255.255
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(UDP_LOG_PORT);

    // Prefisso col nome del nodo, cosi' il server UDP puo' identificare
    // la fonte del log senza doversi basare sull'IP del mittente.
    // Formato del pacchetto: "<NODE_NAME>|<riga di log>"
    char packetBuf[UDP_LOG_BUF_SIZE + 32];
    int prefixLen = snprintf(packetBuf, sizeof(packetBuf), "%s|", NODE_NAME);
    if (prefixLen < 0) {
        prefixLen = 0;
    }
    if (prefixLen >= static_cast<int>(sizeof(packetBuf))) {
        prefixLen = sizeof(packetBuf) - 1;
    }

    int remaining = sizeof(packetBuf) - prefixLen;
    int copyLen = (len < remaining) ? len : (remaining - 1);
    if (copyLen < 0) {
        copyLen = 0;
    }
    memcpy(packetBuf + prefixLen, data, copyLen);
    int totalLen = prefixLen + copyLen;

    // sendto su una socket UDP non bloccante per design (datagram, nessun
    // buffer di invio da attendere); in caso di errore (rete non pronta,
    // nessuna route, ecc.) ritorna -1 e qui lo ignoriamo deliberatamente:
    // il logging su console è già avvenuto, quindi nessuna informazione
    // viene persa per l'operatore locale.
    sendto(s_sockfd, packetBuf, totalLen, 0,
           reinterpret_cast<struct sockaddr*>(&destAddr), sizeof(destAddr));
}