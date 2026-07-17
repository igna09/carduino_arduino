#pragma once

// Pagina servita su GET /. Tenuta come singolo file HTML con CSS/JS inline
// per semplicita' (nessun filesystem SPIFFS/LittleFS necessario).
//
// Logica:
//  - alla connessione apre /events (SSE) e riceve prima la history (righe
//    pregresse), poi il live stream
//  - i filtri (testo, modalita' messaggi TX/RX a 3 stati, lista nodi) sono
//    applicati lato server, prima del ring buffer: cambiare filtro pulisce
//    la vista e riapre la SSE per ricevere una history coerente col nuovo
//    filtro
//  - modalita' messaggi: tutti / solo TX/RX / tutto tranne TX/RX (msgmode
//    0/1/2 inviato a /filter)
//  - filtro nodi: nessuna checkbox nodo selezionata significa "non
//    registrare nulla" (nodes=__NONE__ inviato al server); tutte
//    selezionate significa nessuna restrizione (nodes='')
//  - lo stato del filtro (testo/msgmode/nodi) e' letto da GET /filter
//    all'avvio per prevalorizzare i controlli
//  - la lista dei nodi noti e' letta da GET /nodes e ripopolata
//    periodicamente man mano che il firmware scopre nuovi nodi dai log
//  - il pulsante "Pulisci" svuota sia la vista locale sia il ring buffer
//    lato firmware (GET /clear)
//  - auto-scroll con possibilita' di "congelare" lo scroll
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

  .filterRow2 {
    display: flex;
    align-items: center;
    gap: 10px 14px;
    padding: 8px 14px;
    background: var(--bg-panel);
    border-bottom: 1px solid var(--border);
    flex-wrap: wrap;
    font-size: 12px;
  }
  .filterRow2 label {
    display: flex;
    align-items: center;
    gap: 4px;
    cursor: pointer;
    white-space: nowrap;
    color: var(--fg);
  }
  .filterRow2 input[type=checkbox],
  .filterRow2 input[type=radio] {
    accent-color: var(--accent);
    cursor: pointer;
    flex-shrink: 0;
  }
  .msgModeGroup {
    display: flex;
    align-items: center;
    gap: 14px;
    flex-wrap: wrap;
  }
  .nodesLabel {
    color: var(--fg-dim);
    white-space: nowrap;
  }
  #nodeChecks {
    display: flex;
    align-items: center;
    gap: 10px 12px;
    flex-wrap: wrap;
    min-width: 0;
  }
  .sep {
    width: 1px;
    align-self: stretch;
    background: var(--border);
  }

  /* Su schermi stretti: ogni gruppo logico va a capo su riga propria
     invece di spezzarsi a meta' (es. "Nodi:" isolato da le checkbox) */
  @media (max-width: 640px) {
    .filterRow2 {
      align-items: flex-start;
    }
    .filterRow2 > .msgModeGroup {
      flex: 1 1 100%;
    }
    .sep {
      display: none;
    }
    .nodesGroup {
      flex: 1 1 100%;
      display: flex;
      align-items: flex-start;
      gap: 8px;
    }
    .nodesLabel {
      padding-top: 3px;
    }
    #nodeChecks {
      flex: 1;
    }
  }

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

<div class="filterRow2">
  <div class="msgModeGroup">
    <label><input type="radio" name="msgMode" id="msgModeAll" value="all" checked> Tutti i messaggi</label>
    <label><input type="radio" name="msgMode" id="msgModeOnly" value="only"> Solo messaggi ricevuti/inviati</label>
    <label><input type="radio" name="msgMode" id="msgModeExclude" value="exclude"> Tutto tranne messaggi ricevuti/inviati</label>
  </div>
  <div class="sep"></div>
  <div class="nodesGroup">
    <span class="nodesLabel">Nodi:</span>
    <div id="nodeChecks"><span style="color: var(--fg-dim);">nessun nodo rilevato ancora...</span></div>
  </div>
</div>

<div id="log"></div>

<script>
(function () {
  const logEl = document.getElementById('log');
  const statusEl = document.getElementById('status');
  const filterInput = document.getElementById('filterInput');
  const msgModeAll = document.getElementById('msgModeAll');
  const msgModeOnly = document.getElementById('msgModeOnly');
  const msgModeExclude = document.getElementById('msgModeExclude');
  const nodeChecksEl = document.getElementById('nodeChecks');
  const pauseBtn = document.getElementById('pauseBtn');
  const clearBtn = document.getElementById('clearBtn');
  const countEl = document.getElementById('count');

  const MAX_DOM_ROWS = 2000; // limite righe nel DOM per non appesantire il browser
  let autoScroll = true;
  let rowCount = 0;
  let es = null;
  let filterDebounceTimer = null;

  // Stato del filtro nodi:
  //  - 'all'  -> nessuna restrizione, il server accetta log da qualsiasi nodo
  //              (anche quelli non ancora scoperti); e' lo stato di default.
  //  - 'none' -> NESSUN nodo selezionato: il server non deve registrare nulla.
  //  - 'list' -> solo i nodi in selectedNodes.
  let nodeFilterMode = 'all';
  let selectedNodes = new Set();
  let knownNodes = [];
  let applyingFromServer = false; // evita reconnect ricorsivi durante il caricamento iniziale

  function classify(line) {
    if (/\berr(or)?\b/i.test(line)) return 'match-e';
    if (/\bwarn(ing)?\b/i.test(line)) return 'match-w';
    return '';
  }

  // NB: i filtri sono applicati lato server (prima del ring buffer),
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

  // Costruisce la stringa nodi da inviare al server:
  //  - '' (vuota) quando nodeFilterMode === 'all' -> nessuna restrizione
  //  - '__NONE__' quando nodeFilterMode === 'none' -> blocca tutto
  //  - lista CSV quando nodeFilterMode === 'list'
  function buildNodesParam() {
    if (nodeFilterMode === 'all') return '';
    if (nodeFilterMode === 'none') return '__NONE__';
    return Array.from(selectedNodes).join(',');
  }

  // Invia il filtro corrente al firmware. Le righe scartate da questo
  // momento in poi non verranno piu' salvate nel ring buffer lato ESP32.
  function sendFilterToServer() {
    const text = encodeURIComponent(filterInput.value.trim());
    let msgmode = 0; // 0=tutti, 1=solo TX/RX, 2=tutto tranne TX/RX
    if (msgModeOnly.checked) msgmode = 1;
    else if (msgModeExclude.checked) msgmode = 2;
    const nodes = encodeURIComponent(buildNodesParam());
    return fetch(`/filter?text=${text}&msgmode=${msgmode}&nodes=${nodes}`).catch(() => {
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

  function onFilterControlChanged() {
    if (applyingFromServer) return;
    clearTimeout(filterDebounceTimer);
    filterDebounceTimer = setTimeout(() => {
      sendFilterToServer().then(resetAndReconnect);
    }, 400);
  }

  filterInput.addEventListener('input', onFilterControlChanged);
  msgModeAll.addEventListener('change', onFilterControlChanged);
  msgModeOnly.addEventListener('change', onFilterControlChanged);
  msgModeExclude.addEventListener('change', onFilterControlChanged);

  // Ridisegna le checkbox dei nodi mantenendo lo stato di selezione corrente.
  function renderNodeCheckboxes() {
    if (knownNodes.length === 0) {
      nodeChecksEl.innerHTML = '<span style="color: var(--fg-dim);">nessun nodo rilevato ancora...</span>';
      return;
    }
    nodeChecksEl.innerHTML = '';
    knownNodes.forEach(name => {
      const id = 'node_' + name;
      const checked = selectedNodes.has(name);
      const label = document.createElement('label');
      const cb = document.createElement('input');
      cb.type = 'checkbox';
      cb.id = id;
      cb.dataset.node = name;
      cb.checked = checked;
      cb.addEventListener('change', () => {
        if (cb.checked) selectedNodes.add(name);
        else selectedNodes.delete(name);

        if (selectedNodes.size === 0) {
          nodeFilterMode = 'none';
        } else if (selectedNodes.size === knownNodes.length) {
          nodeFilterMode = 'all';
        } else {
          nodeFilterMode = 'list';
        }
        onFilterControlChanged();
      });
      label.appendChild(cb);
      label.appendChild(document.createTextNode(' ' + name));
      nodeChecksEl.appendChild(label);
    });
  }

  // Interroga /nodes per scoprire nuovi nodi apparsi nei log. I nuovi nodi
  // vengono aggiunti a selectedNodes SOLO se il filtro nodi corrente e'
  // 'all' (nessuna restrizione esplicita); se e' 'none' o 'list' i nuovi
  // nodi restano deselezionati, per non alterare una scelta esplicita
  // dell'utente (incluso il caso "nessun nodo selezionato = non loggare").
  function refreshNodeCheckboxes() {
    fetch('/nodes')
      .then(r => r.ok ? r.json() : [])
      .then(nodes => {
        if (!Array.isArray(nodes)) return;

        if (nodeFilterMode === 'all') {
          nodes.forEach(n => selectedNodes.add(n));
        }

        knownNodes = nodes;
        renderNodeCheckboxes();
      })
      .catch(() => { /* firmware momentaneamente non raggiungibile, riprovo al prossimo giro */ });
  }

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
    // Svuota anche il ring buffer lato firmware: senza questa chiamata la
    // history verrebbe ri-mostrata alla prossima riconnessione SSE.
    fetch('/clear').catch(() => {
      statusEl.textContent = 'errore svuotamento buffer';
    });
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

  // All'avvio: leggo lo stato del filtro gia' impostato lato firmware (es.
  // rimasto da una sessione precedente) e prevalorizzo i controlli, poi
  // carico la lista nodi noti e infine mi connetto alla SSE per ricevere
  // la history gia' coerente con quel filtro.
  applyingFromServer = true;
  fetch('/filter')
    .then(r => r.ok ? r.json() : {})
    .then(state => {
      filterInput.value = state.text || '';
      const mode = state.msgmode || 0;
      msgModeAll.checked = (mode === 0);
      msgModeOnly.checked = (mode === 1);
      msgModeExclude.checked = (mode === 2);

      const nodesStr = (state.nodes || '').trim();
      if (nodesStr === '') {
        nodeFilterMode = 'all';
        selectedNodes = new Set();
      } else if (nodesStr === '__NONE__') {
        nodeFilterMode = 'none';
        selectedNodes = new Set();
      } else {
        nodeFilterMode = 'list';
        selectedNodes = new Set(nodesStr.split(',').filter(Boolean));
      }
    })
    .catch(() => { /* firmware non raggiungibile per la lettura, procedo comunque */ })
    .then(() => fetch('/nodes').then(r => r.ok ? r.json() : []).catch(() => []))
    .then(nodes => {
      knownNodes = Array.isArray(nodes) ? nodes : [];
      // Se il filtro nodi e' 'all' (nessuna restrizione salvata), tutti i
      // nodi noti risultano selezionati nella UI, come richiesto.
      if (nodeFilterMode === 'all') {
        selectedNodes = new Set(knownNodes);
      }
      renderNodeCheckboxes();
    })
    .finally(() => {
      applyingFromServer = false;
      connect();
      setInterval(refreshNodeCheckboxes, 5000);
    });
})();
</script>

</body>
</html>
)HTML_PAGE";