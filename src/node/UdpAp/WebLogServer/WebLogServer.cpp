#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "esp_http_server.h"

#include "WebLogServer.h"
#include "WebLogServerIndexHtml.h"
#include "NodeLog.h"

namespace WebLogServer {

#define WEB_LOG_LINE_MAX     160
#define WEB_LOG_RING_SIZE    800
#define WEB_LOG_MAX_CLIENTS  4   
#define WEB_LOG_ASYNC_WORKERS WEB_LOG_MAX_CLIENTS

static char s_ring[WEB_LOG_RING_SIZE][WEB_LOG_LINE_MAX];
static int s_ringHead = 0;     
static int s_ringCount = 0;    
static SemaphoreHandle_t s_ringMutex = nullptr;
static volatile uint32_t s_seq = 0; 

static int s_clientFds[WEB_LOG_MAX_CLIENTS];
static int s_clientCount = 0;
static SemaphoreHandle_t s_clientsMutex = nullptr;

// --- Filtro server-side: le righe che non combaciano NON entrano nel ring buffer ---
#define WEB_LOG_FILTER_MAX 64

// --- Filtro server-side esteso: testo + solo msg TX/RX + lista nodi ---
static char s_filterText[WEB_LOG_FILTER_MAX] = "";
static bool s_filterMsgOnly = false;                 // checkbox "solo messaggi ricevuti/inviati"
static char s_filterNodesList[WEB_LOG_FILTER_MAX] = ""; // "MainNode,KlineNode" vuoto = tutti
static SemaphoreHandle_t s_filterMutex = nullptr;

// Nodi noti, popolati automaticamente dai prefissi [NodeName] visti
#define WEB_LOG_MAX_KNOWN_NODES 16
#define WEB_LOG_NODE_NAME_MAX   32
static char s_knownNodes[WEB_LOG_MAX_KNOWN_NODES][WEB_LOG_NODE_NAME_MAX];
static int s_knownNodeCount = 0;
static SemaphoreHandle_t s_nodesMutex = nullptr;

static void registerKnownNode(const char* name) {
    if (name == nullptr || name[0] == '\0') return;
    xSemaphoreTake(s_nodesMutex, portMAX_DELAY);
    for (int i = 0; i < s_knownNodeCount; i++) {
        if (strcmp(s_knownNodes[i], name) == 0) { xSemaphoreGive(s_nodesMutex); return; }
    }
    if (s_knownNodeCount < WEB_LOG_MAX_KNOWN_NODES) {
        strlcpy(s_knownNodes[s_knownNodeCount++], name, WEB_LOG_NODE_NAME_MAX);
    }
    xSemaphoreGive(s_nodesMutex);
}

// Confronto case-insensitive "haystack contiene needle", senza allocazioni
static bool containsCaseInsensitive(const char* haystack, const char* needle) {
    if (needle[0] == '\0') return true; // filtro vuoto = passa tutto
    size_t hn = strlen(haystack), nn = strlen(needle);
    if (nn > hn) return false;
    for (size_t i = 0; i + nn <= hn; i++) {
        size_t j = 0;
        for (; j < nn; j++) {
            char a = haystack[i + j], b = needle[j];
            if (a >= 'A' && a <= 'Z') a += 32;
            if (b >= 'A' && b <= 'Z') b += 32;
            if (a != b) break;
        }
        if (j == nn) return true;
    }
    return false;
}

static bool passesServerFilter(const char* nodeName, const char* msg, const char* fullLine) {
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);

    bool textOk = containsCaseInsensitive(fullLine, s_filterText);

    bool msgOnlyOk = true;
    if (s_filterMsgOnly) {
        msgOnlyOk = containsCaseInsensitive(msg, "received message") ||
                    containsCaseInsensitive(msg, "sent message");
    }

    bool nodeOk = true;
    if (s_filterNodesList[0] != '\0') {
        nodeOk = false;
        char tmp[WEB_LOG_FILTER_MAX];
        strlcpy(tmp, s_filterNodesList, sizeof(tmp));
        char* tok = strtok(tmp, ",");
        while (tok != nullptr) {
            if (nodeName != nullptr && strcasecmp(tok, nodeName) == 0) { nodeOk = true; break; }
            tok = strtok(nullptr, ",");
        }
    }

    xSemaphoreGive(s_filterMutex);
    return textOk && msgOnlyOk && nodeOk;
}

static void setFilter(const char* text, bool msgOnly, const char* nodesList) {
    xSemaphoreTake(s_filterMutex, portMAX_DELAY);
    strlcpy(s_filterText, text, WEB_LOG_FILTER_MAX);
    s_filterMsgOnly = msgOnly;
    strlcpy(s_filterNodesList, nodesList, WEB_LOG_FILTER_MAX);
    xSemaphoreGive(s_filterMutex);
    NLOGI("[Filter] text=\"%s\" msgOnly=%d nodes=\"%s\"", text, msgOnly, nodesList);
}

static httpd_handle_t s_server = nullptr;
static TaskHandle_t s_asyncWorkerTasks[WEB_LOG_ASYNC_WORKERS];

struct AsyncEventsRequest {
    httpd_req_t* req;
};

static QueueHandle_t s_asyncEventsQueue = nullptr;
static SemaphoreHandle_t s_asyncWorkerFree = nullptr; 

// --- Ring buffer: scrittura ---
static void ringPush(const char* line) {
    xSemaphoreTake(s_ringMutex, portMAX_DELAY);
    strlcpy(s_ring[s_ringHead], line, WEB_LOG_LINE_MAX);
    s_ringHead = (s_ringHead + 1) % WEB_LOG_RING_SIZE;
    if (s_ringCount < WEB_LOG_RING_SIZE) {
        s_ringCount++;
    }
    xSemaphoreGive(s_ringMutex);
}

// --- Registrazione/rimozione client SSE ---
static void clientAdd(int fd) {
    xSemaphoreTake(s_clientsMutex, portMAX_DELAY);
    if (s_clientCount < WEB_LOG_MAX_CLIENTS) {
        s_clientFds[s_clientCount++] = fd;
        NLOGI("[FD %d] Client registrato nell'elenco live. Client attuali: %d/%d", fd, s_clientCount, WEB_LOG_MAX_CLIENTS);
    } else {
        NLOGW("Numero massimo di client web (%d) raggiunto, connessione live rifiutata per FD %d", WEB_LOG_MAX_CLIENTS, fd);
    }
    xSemaphoreGive(s_clientsMutex);
}

static void clientRemove(int fd) {
    xSemaphoreTake(s_clientsMutex, portMAX_DELAY);
    for (int i = 0; i < s_clientCount; i++) {
        if (s_clientFds[i] == fd) {
            s_clientFds[i] = s_clientFds[s_clientCount - 1];
            s_clientCount--;
            NLOGI("[FD %d] Client rimosso dall'elenco live. Client rimanenti: %d", fd, s_clientCount);
            break;
        }
    }
    xSemaphoreGive(s_clientsMutex);
}

static bool sendSseChunk(httpd_handle_t server, int fd, const char* line) {
    char chunk[WEB_LOG_LINE_MAX + 16];
    int n = snprintf(chunk, sizeof(chunk), "data: %s\n\n", line);
    if (n <= 0) {
        return true; 
    }
    // httpd_socket_send restituisce i byte inviati, verifichiamo che sia andato a buon fine (>= 0)
    int ret = httpd_socket_send(server, fd, chunk, n, 0);
    if (ret < 0) {
        NLOGE("[FD %d] Fallito invio pacchetto SSE (ret: %d)", fd, ret);
    }
    return ret >= 0;
}

// --- Loop eseguito dal Worker Asincrono ---
static void runSseSession(httpd_req_t* req) {
    int fd = httpd_req_to_sockfd(req);
    httpd_handle_t server = req->handle;

    NLOGI("[FD %d] Avvio runSseSession per il client log", fd);

    // CORREZIONE: Invio manuale degli header sul socket per completare l'handshake SSE
    const char* httpHeaders = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/event-stream\r\n"
        "Cache-Control: no-cache\r\n"
        "Connection: keep-alive\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "\r\n"; 

    // esp_err_t err = httpd_socket_send(server, fd, httpHeaders, strlen(httpHeaders), 0);
    // if (err != ESP_OK) {
    //     NLOGE("[FD %d] Errore critico nell'invio degli HTTP Headers SSE: %s", fd, esp_err_to_name(err));
    //     return; 
    // }
    // NLOGI("[FD %d] Handshake SSE inviato con successo", fd);
    // Modifica il tipo da esp_err_t a int
    int bytesSent = httpd_socket_send(server, fd, httpHeaders, strlen(httpHeaders), 0);
    if (bytesSent < 0) {
        NLOGE("[FD %d] Errore critico nell'invio degli HTTP Headers SSE (ret: %d)", fd, bytesSent);
        return; 
    }
    NLOGI("[FD %d] Handshake SSE inviato con successo (%d byte)", fd, bytesSent);

    char lineBuf[WEB_LOG_LINE_MAX];

    // --- Invia history corrente ---
    xSemaphoreTake(s_ringMutex, portMAX_DELAY);
    int count = s_ringCount;
    int startIdx = (s_ringHead - count + WEB_LOG_RING_SIZE) % WEB_LOG_RING_SIZE;
    xSemaphoreGive(s_ringMutex);

    NLOGI("[FD %d] Invio della cronologia corrente (%d linee log)", fd, count);
    for (int i = 0; i < count; i++) {
        xSemaphoreTake(s_ringMutex, portMAX_DELAY);
        int idx = (startIdx + i) % WEB_LOG_RING_SIZE;
        strlcpy(lineBuf, s_ring[idx], WEB_LOG_LINE_MAX);
        xSemaphoreGive(s_ringMutex);

        if (!sendSseChunk(server, fd, lineBuf)) {
            NLOGW("[FD %d] Interruzione invio cronologia causa disconnessione client", fd);
            return; 
        }
    }
    NLOGI("[FD %d] Cronologia inviata. Entro nel loop live stream", fd);

    // --- Passa in modalita' live stream ---
    clientAdd(fd);
    uint32_t lastSeq = s_seq;
    uint32_t heartbeatTicks = 0; 

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(150));

        uint32_t curSeq = s_seq;
        if (curSeq == lastSeq) {
            // Monitora la connessione inviando un ping ogni ~3 secondi se non ci sono log nuovi
            heartbeatTicks++;
            if (heartbeatTicks >= 20) { 
                heartbeatTicks = 0;
                int ret = httpd_socket_send(server, fd, ": ping\n\n", 8, 0);
                if (ret < 0) { // < 0 significa socket chiuso o errore di trasmissione
                    NLOGW("[FD %d] Heartbeat fallito (ret: %d). Il client ha chiuso la pagina web.", fd, ret);
                    clientRemove(fd);
                    return; 
                }
            }
            continue; 
        }

        heartbeatTicks = 0;

        uint32_t newCount = curSeq - lastSeq;
        NLOGI("[FD %d] Rilevati %d nuovi messaggi di log (Seq: %d -> %d)", fd, newCount, lastSeq, curSeq);
        
        if (newCount > (uint32_t)WEB_LOG_RING_SIZE) {
            NLOGW("[FD %d] Overflow rilevato, i messaggi superano il ring buffer. Cap impostato a %d", fd, WEB_LOG_RING_SIZE);
            newCount = WEB_LOG_RING_SIZE;
        }

        int liveStart = (s_ringHead - (int)newCount + WEB_LOG_RING_SIZE) % WEB_LOG_RING_SIZE;
        for (uint32_t i = 0; i < newCount; i++) {
            xSemaphoreTake(s_ringMutex, portMAX_DELAY);
            int idx = (liveStart + (int)i) % WEB_LOG_RING_SIZE;
            strlcpy(lineBuf, s_ring[idx], WEB_LOG_LINE_MAX);
            xSemaphoreGive(s_ringMutex);

            if (!sendSseChunk(server, fd, lineBuf)) {
                NLOGE("[FD %d] Errore durante l'invio del messaggio live, chiudo sessione client", fd);
                clientRemove(fd);
                return;
            }
        }

        lastSeq = curSeq;
    }
}

static void asyncEventsWorkerTask(void* arg) {
    AsyncEventsRequest item;
    NLOGI("Background Worker Task SSE avviato con successo");
    
    while (1) {
        if (xQueueReceive(s_asyncEventsQueue, &item, portMAX_DELAY) != pdTRUE) {
            continue;
        }

        NLOGI("Worker estratto una richiesta dalla coda, inizio elaborazione sessione");
        runSseSession(item.req);

        NLOGI("Sessione terminata. Rilascio risorse della richiesta asincrona");
        httpd_req_async_handler_complete(item.req);
        xSemaphoreGive(s_asyncWorkerFree);
    }
}

static esp_err_t indexGetHandler(httpd_req_t* req) {
    NLOGI("Richiesta GET su '/' ricevuta, invio della pagina statica");
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, WEB_LOG_INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

// --- Download storico completo come file di testo (streaming a chunk) ---
static esp_err_t downloadGetHandler(httpd_req_t* req) {
    NLOGI("Richiesta GET su '/download' ricevuta, invio storico log completo");

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Content-Disposition", "attachment; filename=\"carduino_log.txt\"");

    xSemaphoreTake(s_ringMutex, portMAX_DELAY);
    int count = s_ringCount;
    int startIdx = (s_ringHead - count + WEB_LOG_RING_SIZE) % WEB_LOG_RING_SIZE;
    xSemaphoreGive(s_ringMutex);

    char lineBuf[WEB_LOG_LINE_MAX + 2]; // +2 per "\n"
    for (int i = 0; i < count; i++) {
        xSemaphoreTake(s_ringMutex, portMAX_DELAY);
        int idx = (startIdx + i) % WEB_LOG_RING_SIZE;
        int n = snprintf(lineBuf, sizeof(lineBuf), "%s\n", s_ring[idx]);
        xSemaphoreGive(s_ringMutex);

        if (httpd_resp_send_chunk(req, lineBuf, n) != ESP_OK) {
            NLOGW("[Download] Client disconnesso durante l'invio, interruzione (linea %d/%d)", i, count);
            httpd_resp_send_chunk(req, nullptr, 0);
            return ESP_FAIL;
        }
    }

    httpd_resp_send_chunk(req, nullptr, 0);
    NLOGI("[Download] Storico inviato (%d linee)", count);
    return ESP_OK;
}

static esp_err_t filterGetHandler(httpd_req_t* req) {
    char query[192];
    bool hasQuery = (httpd_req_get_url_query_len(req) > 0 &&
                      httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK);

    if (!hasQuery) {
        char text[WEB_LOG_FILTER_MAX], nodes[WEB_LOG_FILTER_MAX];
        bool msgOnly;
        xSemaphoreTake(s_filterMutex, portMAX_DELAY);
        strlcpy(text, s_filterText, sizeof(text));
        strlcpy(nodes, s_filterNodesList, sizeof(nodes));
        msgOnly = s_filterMsgOnly;
        xSemaphoreGive(s_filterMutex);

        char resp[400];
        snprintf(resp, sizeof(resp), "{\"text\":\"%s\",\"msgonly\":%d,\"nodes\":\"%s\"}",
                 text, msgOnly ? 1 : 0, nodes);
        httpd_resp_set_type(req, "application/json");
        return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    }

    char textParam[WEB_LOG_FILTER_MAX] = "";
    char nodesParam[WEB_LOG_FILTER_MAX] = "";
    char msgOnlyParam[4] = "0";
    httpd_query_key_value(query, "text", textParam, sizeof(textParam));
    httpd_query_key_value(query, "nodes", nodesParam, sizeof(nodesParam));
    httpd_query_key_value(query, "msgonly", msgOnlyParam, sizeof(msgOnlyParam));

    for (char* p = textParam; *p; p++) if (*p == '+') *p = ' ';

    setFilter(textParam, atoi(msgOnlyParam) != 0, nodesParam);

    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_send(req, "OK", HTTPD_RESP_USE_STRLEN);
}

static esp_err_t nodesGetHandler(httpd_req_t* req) {
    char resp[WEB_LOG_MAX_KNOWN_NODES * (WEB_LOG_NODE_NAME_MAX + 3) + 8] = "[";
    xSemaphoreTake(s_nodesMutex, portMAX_DELAY);
    for (int i = 0; i < s_knownNodeCount; i++) {
        strlcat(resp, "\"", sizeof(resp));
        strlcat(resp, s_knownNodes[i], sizeof(resp));
        strlcat(resp, "\"", sizeof(resp));
        if (i < s_knownNodeCount - 1) strlcat(resp, ",", sizeof(resp));
    }
    xSemaphoreGive(s_nodesMutex);
    strlcat(resp, "]", sizeof(resp));

    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t eventsGetHandler(httpd_req_t* req) {
    NLOGI("Richiesta GET su '/events' (SSE) intercettata. Avvio logica asincrona");
    
    httpd_req_t* asyncReq = nullptr;
    esp_err_t err = httpd_req_async_handler_begin(req, &asyncReq);
    if (err != ESP_OK) {
        NLOGE("Impossibile staccare la richiesta per la gestione asincrona: %s", esp_err_to_name(err));
        return err;
    }

    if (xSemaphoreTake(s_asyncWorkerFree, 0) != pdTRUE) {
        NLOGW("Nessun Worker SSE libero disponibile (Client Massimi: %d). Connessione rifiutata.", WEB_LOG_ASYNC_WORKERS);
        httpd_req_async_handler_complete(asyncReq);
        return ESP_FAIL;
    }

    AsyncEventsRequest item = { asyncReq };
    if (xQueueSend(s_asyncEventsQueue, &item, pdMS_TO_TICKS(100)) != pdTRUE) {
        NLOGE("La coda s_asyncEventsQueue è piena, impossibile delegare la richiesta");
        xSemaphoreGive(s_asyncWorkerFree);
        httpd_req_async_handler_complete(asyncReq);
        return ESP_FAIL;
    }

    NLOGI("Richiesta registrata nella coda ed associata correttamente al pool di worker");
    return ESP_OK;
}

void pushLine(const char* line) {
    if (line == nullptr) return;

    // Estrae il nodeName da "[Node]: msg" se presente, per coerenza col filtro nodi
    const char* nodeName = nullptr;
    const char* msg = line;
    char nodeBuf[WEB_LOG_NODE_NAME_MAX];
    if (line[0] == '[') {
        const char* close = strchr(line, ']');
        if (close != nullptr) {
            size_t nlen = MIN((size_t)(close - line - 1), sizeof(nodeBuf) - 1);
            memcpy(nodeBuf, line + 1, nlen);
            nodeBuf[nlen] = 0;
            nodeName = nodeBuf;
            msg = (close[1] == ':' && close[2] == ' ') ? close + 3 : close + 1;
        }
    }

    if (nodeName != nullptr) registerKnownNode(nodeName);
    if (!passesServerFilter(nodeName, msg, line)) return;
    ringPush(line);
    s_seq = s_seq + 1;
}

void pushLineFormatted(const char* prefix, const char* msg) {
    if (prefix == nullptr) prefix = "";
    if (msg == nullptr) msg = "";

    registerKnownNode(prefix);

    char line[WEB_LOG_LINE_MAX];
    snprintf(line, sizeof(line), "[%s]: %s", prefix, msg);

    if (!passesServerFilter(prefix, msg, line)) return;
    ringPush(line);
    s_seq = s_seq + 1;
}

// CORREZIONE: Inizializzazione completa delle code e dei Task in background
void init() {
    s_nodesMutex = xSemaphoreCreateMutex();
    s_filterMsgOnly = false;
    s_filterNodesList[0] = '\0';
    s_ringMutex = xSemaphoreCreateMutex();
    s_clientsMutex = xSemaphoreCreateMutex();
    s_filterMutex = xSemaphoreCreateMutex();
    s_clientCount = 0;
    s_filterText[0] = '\0';

    // Crea le risorse FreeRTOS per la gestione asincrona
    s_asyncEventsQueue = xQueueCreate(WEB_LOG_ASYNC_WORKERS, sizeof(AsyncEventsRequest));
    s_asyncWorkerFree = xSemaphoreCreateCounting(WEB_LOG_ASYNC_WORKERS, WEB_LOG_ASYNC_WORKERS);

    // Genera fisicamente i task dei worker in background
    for (int i = 0; i < WEB_LOG_ASYNC_WORKERS; i++) {
        char taskName[16];
        snprintf(taskName, sizeof(taskName), "web_sse_w_%d", i);
        xTaskCreate(asyncEventsWorkerTask, taskName, 4096, nullptr, 5, &s_asyncWorkerTasks[i]);
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = WEB_LOG_MAX_CLIENTS + 1; 
    config.lru_purge_enable = true;
    config.stack_size = 6144;

    esp_err_t err = httpd_start(&s_server, &config);
    if (err != ESP_OK) {
        NLOGE("Avvio server HTTP web log fallito: %s", esp_err_to_name(err));
        return;
    }

    httpd_uri_t indexUri = {};
    indexUri.uri = "/";
    indexUri.method = HTTP_GET;
    indexUri.handler = indexGetHandler;
    httpd_register_uri_handler(s_server, &indexUri);

    httpd_uri_t eventsUri = {};
    eventsUri.uri = "/events";
    eventsUri.method = HTTP_GET;
    eventsUri.handler = eventsGetHandler;
    httpd_register_uri_handler(s_server, &eventsUri);

    httpd_uri_t downloadUri = {};
    downloadUri.uri = "/download";
    downloadUri.method = HTTP_GET;
    downloadUri.handler = downloadGetHandler;
    httpd_register_uri_handler(s_server, &downloadUri);

    httpd_uri_t filterUri = {};
    filterUri.uri = "/filter";
    filterUri.method = HTTP_GET;
    filterUri.handler = filterGetHandler;
    httpd_register_uri_handler(s_server, &filterUri);

    httpd_uri_t nodesUri = {};
    nodesUri.uri = "/nodes";
    nodesUri.method = HTTP_GET;
    nodesUri.handler = nodesGetHandler;
    httpd_register_uri_handler(s_server, &nodesUri);

    NLOGI("Web log server avviato su http://192.168.4.1/ (max %d client SSE)", WEB_LOG_MAX_CLIENTS);
}

} // namespace WebLogServer