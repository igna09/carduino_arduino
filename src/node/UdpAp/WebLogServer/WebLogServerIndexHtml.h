#pragma once

// Pagina servita su GET /. Tenuta come singolo file HTML con CSS/JS inline
// per semplicita' (nessun filesystem SPIFFS/LittleFS necessario).
//
// Logica:
//  - alla connessione apre /events (SSE) e riceve prima la history (righe
//    pregresse, marcate con campo "h"), poi il live stream (campo "l")
//  - il filtro testo (nome nodo o contenuto messaggio, case-insensitive)
//    e' applicato lato client: le righe filtrate via JS, nessun round-trip
//  - auto-scroll con possibilita' di "congelare" lo scroll (utile quando si
//    vuole leggere senza che il flusso continuo sposti la vista)
static const char WEB_LOG_INDEX_HTML[] = R"HTML_PAGE(<!DOCTYPE html>
<html lang="it">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Carduino - Log UDP</title>
<style>
  :root {
    --bg: #0d1117;
    --bg-panel: #11161d;
    --fg: #c9d1d9;
    --fg-dim: #6e7681;
    --accent: #58a6ff;
    --ok: #3fb950;
    --warn: #d29922;
    --err: #f85149;
    --border: #21262d;
  }
  * { box-sizing: border-box; }
  body {
    margin: 0;
    background: var(--bg);
    color: var(--fg);
    font-family: ui-monospace, "SF Mono", "Cascadia Code", Consolas, monospace;
    font-size: 13px;
    height: 100vh;
    display: flex;
    flex-direction: column;
  }
  header {
    display: flex;
    align-items: center;
    gap: 10px;
    padding: 10px 14px;
    background: var(--bg-panel);
    border-bottom: 1px solid var(--border);
    flex-wrap: wrap;
  }
  header h1 {
    font-size: 14px;
    font-weight: 600;
    margin: 0;
    color: var(--accent);
    white-space: nowrap;
  }
  #status {
    font-size: 11px;
    padding: 2px 8px;
    border-radius: 10px;
    white-space: nowrap;
  }
  #status.connected { background: rgba(63,185,80,0.15); color: var(--ok); }
  #status.disconnected { background: rgba(248,81,73,0.15); color: var(--err); }
  #filterInput {
    flex: 1;
    min-width: 160px;
    background: var(--bg);
    color: var(--fg);
    border: 1px solid var(--border);
    border-radius: 6px;
    padding: 6px 10px;
    font-family: inherit;
    font-size: 13px;
  }
  #filterInput:focus { outline: none; border-color: var(--accent); }
  button {
    background: var(--bg);
    color: var(--fg);
    border: 1px solid var(--border);
    border-radius: 6px;
    padding: 6px 10px;
    font-family: inherit;
    font-size: 12px;
    cursor: pointer;
    white-space: nowrap;
  }
  button:hover { border-color: var(--accent); color: var(--accent); }
  button.active { border-color: var(--accent); color: var(--accent); background: rgba(88,166,255,0.1); }
  #count { font-size: 11px; color: var(--fg-dim); white-space: nowrap; }
  #log {
    flex: 1;
    overflow-y: auto;
    padding: 8px 14px;
    line-height: 1.5;
  }
  .row { white-space: pre-wrap; word-break: break-word; }
  .row .node { color: var(--accent); }
  .row .ts { color: var(--fg-dim); margin-right: 6px; }
  .row.hidden { display: none; }
  .row.match-w .node { color: var(--warn); }
  .row.match-e .node { color: var(--err); }
  #log::-webkit-scrollbar { width: 10px; }
  #log::-webkit-scrollbar-thumb { background: var(--border); border-radius: 5px; }
</style>
</head>
<body>

<header>
  <h1>Carduino &middot; Log UDP</h1>
  <span id="status" class="disconnected">disconnesso</span>
  <input id="filterInput" type="text" placeholder="Filtra per nodo o testo (es. Klein, ERR, batteria)...">
  <button id="pauseBtn">Pausa scroll</button>
  <button id="clearBtn">Pulisci</button>
  <a id="downloadBtn" href="/download" download="carduino_log.txt"><button type="button">Scarica log</button></a>
  <span id="count">0 righe</span>
</header>

<div id="log"></div>

<script>
(function () {
  const logEl = document.getElementById('log');
  const statusEl = document.getElementById('status');
  const filterInput = document.getElementById('filterInput');
  const pauseBtn = document.getElementById('pauseBtn');
  const clearBtn = document.getElementById('clearBtn');
  const countEl = document.getElementById('count');

  const MAX_DOM_ROWS = 2000; // limite righe nel DOM per non appesantire il browser
  let autoScroll = true;
  let rowCount = 0;
  let es = null;
  let filterDebounceTimer = null;

  function classify(line) {
    if (/\berr(or)?\b/i.test(line)) return 'match-e';
    if (/\bwarn(ing)?\b/i.test(line)) return 'match-w';
    return '';
  }

  // NB: il filtro ora e' applicato lato server (prima del ring buffer),
  // quindi ogni riga ricevuta qui e' gia' stata accettata dal firmware.
  function appendRow(text) {
    const div = document.createElement('div');
    div.className = 'row ' + classify(text);
    div.textContent = text;
    logEl.appendChild(div);
    rowCount++;

    while (logEl.children.length > MAX_DOM_ROWS) {
      logEl.removeChild(logEl.firstChild);
    }

    countEl.textContent = rowCount + ' righe';

    if (autoScroll) {
      logEl.scrollTop = logEl.scrollHeight;
    }
  }

  // Invia il nuovo filtro al firmware. Le righe scartate da questo momento
  // in poi non verranno piu' salvate nel ring buffer lato ESP32.
  function sendFilterToServer(text) {
    fetch('/filter?text=' + encodeURIComponent(text)).catch(() => {
      statusEl.textContent = 'errore invio filtro';
    });
  }

  // Dopo un cambio filtro puliamo la vista e riapriamo la SSE: la history
  // che arriva ora e' quella gia' filtrata dal server.
  function resetAndReconnect() {
    logEl.innerHTML = '';
    rowCount = 0;
    countEl.textContent = '0 righe';
    if (es) es.close();
    connect();
  }

  filterInput.addEventListener('input', () => {
    const text = filterInput.value.trim();
    clearTimeout(filterDebounceTimer);
    // Debounce per non spammare il server ad ogni tasto premuto
    filterDebounceTimer = setTimeout(() => {
      sendFilterToServer(text);
      resetAndReconnect();
    }, 400);
  });

  pauseBtn.addEventListener('click', () => {
    autoScroll = !autoScroll;
    pauseBtn.classList.toggle('active', !autoScroll);
    pauseBtn.textContent = autoScroll ? 'Pausa scroll' : 'Riprendi scroll';
    if (autoScroll) logEl.scrollTop = logEl.scrollHeight;
  });

  clearBtn.addEventListener('click', () => {
    logEl.innerHTML = '';
    rowCount = 0;
    countEl.textContent = '0 righe';
  });

  function connect() {
    es = new EventSource('/events');

    es.onopen = () => {
      statusEl.textContent = 'connesso';
      statusEl.className = 'connected';
    };

    es.onerror = () => {
      statusEl.textContent = 'disconnesso, riconnessione...';
      statusEl.className = 'disconnected';
    };

    es.onmessage = (ev) => {
      appendRow(ev.data);
    };
  }

  connect();
})();
</script>

</body>
</html>
)HTML_PAGE";