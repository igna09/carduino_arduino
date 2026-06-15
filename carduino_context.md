# Carduino Arduino Project Context

## Overview

The `carduino_arduino` project is a modular embedded system designed for automotive applications, likely to enhance or integrate with a car's existing systems. It utilizes an Arduino-compatible platform and communicates primarily via a CAN bus network. The overall goal is to provide a flexible and extensible platform for integrating custom electronics into a car, offering functionalities such as enhanced media control, car diagnostics, and potentially other custom features.

## Project Structure

The project follows a modular architecture, built around a `CarduinoNode` base class (implied). Each specialized node inherits from this base class, gaining common functionalities while implementing its specific logic.

### Core Components / Specialized Nodes

1.  **`MediaControlCarduinoNode`**
    *   **Purpose:** Handles media control functionalities, typically for a car audio system. It interprets user input from a rotary encoder and translates these actions into analog resistance values to interface with car head units (e.g., for Steering Wheel Controls - SWC).
    *   **Key Features:**
        *   Reads rotary encoder input for actions like VOLUME_UP, VOLUME_DOWN, PLAY_PAUSE, NEXT, LONG_PRESS.
        *   Controls a digital potentiometer (`X9C103S`) to output specific resistance values corresponding to media commands.
        *   Implements an SWC pairing sequence to map physical controls to resistance outputs.
        *   Provides audio feedback via a buzzer for various events (e.g., pairing status, warnings).
        *   Communicates media control events over the CAN bus.
    *   **Dependencies:** `CarduinoNode`, `PCF8574` (I2C I/O expander), `Versatile_RotaryEncoder`, `X9C103S`, `Event`, `MediaControl`, `MediaControlMessage`.

2.  **`KlineCarduinoNode`**
    *   **Purpose:** Acts as a diagnostic and data acquisition unit, communicating with the car's Electronic Control Units (ECUs) using the K-Line (KWP1281) protocol. It reads various car status values and broadcasts them over the CAN bus.
    *   **Key Features:**
        *   Establishes communication with ECUs via `SoftwareSerial` and `KLineKWP1281Lib`.
        *   Periodically reads diagnostic "blocks" and "measurements" (e.g., fuel consumption, engine parameters) from connected ECUs.
        *   Parses measurement types (value, units, unknown).
        *   Sends `CarstatusMessage` objects over the CAN bus containing real-time car data.
        *   Monitors and reports battery voltage.
        *   Handles an OTA (Over-The-Air) mode, disconnecting from K-Line during updates.
    *   **Dependencies:** `CarduinoNode`, `SoftwareSerial`, `KLineKWP1281Lib`, `ValueToReadEnum`, `CarstatusMessage`, `Carstatus`, `CanbusMessageType`, `AfterReadExecutors`, `FuelConsumptionExecutor`.

3.  **Other Implied Nodes:** The project structure suggests the existence of other specialized nodes (e.g., `DoorCarduinoNode`, `Sniffer`, `CommunicationCarduinoNode`, `MainCarduinoNode`, `test`) which likely handle specific car functions or system-level tasks.

### Common Base Class (`CarduinoNode` - Implied)

Both `MediaControlCarduinoNode` and `KlineCarduinoNode` inherit from a `CarduinoNode` base class. This class serves as the foundation for all specialized nodes, providing a generic framework and common functionalities essential for an automotive embedded system.

**Technical Details of the Generic Class System:**

*   **Inheritance Model:** Specialized nodes (e.g., `MediaControlCarduinoNode`, `KlineCarduinoNode`) publicly inherit from `CarduinoNode`. This allows them to reuse common code and enforce a consistent interface across different modules.
*   **Constructor Chaining:** Derived classes call the `CarduinoNode` constructor in their initialization list, passing parameters like `id`, `cs` (chip select for CAN), `interruptPin`, `ssid`, and `password`. This ensures that the base class is properly initialized with fundamental node-specific and network-related configurations.
*   **Polymorphism:** The `loop()` method is a virtual function in `CarduinoNode` (implicitly, as it's overridden by derived classes and called in the main loop). This allows each node to implement its specific operational logic while being managed uniformly by a central scheduler or main loop.
*   **Common Functionalities Provided by `CarduinoNode`:**
    *   **CAN Bus Communication:** Manages the MCP_CAN controller (`mcp_can.h`) for sending and receiving messages on the CAN network. It includes methods like `sendCanbusMessage` and `handleRxBuffer` for message handling. A buffer (`CanMessageValues* messageBuffer`) is used to store incoming CAN messages.
    *   **Settings Management:** Provides mechanisms to `restoreSettings()` and `getSettingValue()` for persistent storage and retrieval of node configurations. This likely involves an EEPROM or Flash-based storage solution.
    *   **Task Scheduling:** Integrates the `TaskScheduler` library (`TaskSchedulerDeclarations.h`) to manage timed and periodic events. Each node has its own `scheduler` instance, allowing for cooperative multitasking without blocking the main loop. Tasks like `heartbeatWdtTask` and `delayTask` are managed here.
    *   **Node Lifecycle Management:** Defines the basic `loop()` method that derived classes extend to perform their specific operations. It also includes `enable()` and `disable()` methods to control the active state of a node.
    *   **Network Connectivity & OTA Updates:** Handles Wi-Fi connectivity (SSID, password) and provides support for Over-The-Air (OTA) firmware updates using `ArduinoOTA` (indicated by `OTAStorage.cpp`). This allows for remote updates of node firmware.
    *   **Logging:** Inherits from `Logger`, providing `printlnWrapper` for consistent logging output, which can be directed to serial or other interfaces.
    *   **File System (FS) Operations:** Inherits from `FSBase`, suggesting capabilities for interacting with a file system (e.g., SPIFFS, LittleFS) for storing configuration files, web assets, or other data.
    *   **Web Server:** Includes `AsyncWebServer` for setting up a web interface, potentially for configuration, monitoring, or serving web applications (as suggested by `FALLBACK_PAGE` and server setup methods).
    *   **Pin Management:** Provides methods like `addPinToRead` and `readDigitalPins` to manage digital I/O, including interaction with I2C I/O expanders like `PCF8574`.
    *   **Event Handling:** Includes mechanisms to `sendEvent` and manage `EventMessage`s over the CAN bus, allowing nodes to communicate significant occurrences.
    *   **Executors:** Utilizes an `Executor` pattern for handling specific CAN events or serial commands, allowing for extensible processing logic. Examples include `CarduinoNodeCanEvent`, `CarduinoNodeCanGetSettings`, etc.

### Flusso di Comunicazione e Gestione Messaggi

Il sistema gestisce lo scambio di dati attraverso due canali principali: il bus CAN e l'interfaccia Serial (USB).

#### Gestione Messaggi in Ingresso

1.  **Messaggi CAN (`manageReceivedCanbusMessage`):**
    *   I messaggi provenienti dal bus fisico vengono letti dal controller MCP2515 e inseriti in un buffer circolare (`messageBuffer`).
    *   Il metodo `handleRxBuffer` processa periodicamente questo buffer estraendo i messaggi.
    *   Ogni messaggio viene passato a `manageReceivedCanbusMessage`, che delega l'esecuzione al `canExecutor`. Questo componente identifica la logica corretta da applicare in base all'ID e alla Categoria del messaggio (es. aggiornamento impostazioni o gestione eventi di stato).

2.  **Messaggi Seriali/USB (`handleReceivedSerialMessage`):**
    *   I messaggi ricevuti via Serial seguono un formato testuale separato da punti e virgola: `Categoria;Enum;Valore` (es. `HEARTBEAT;0;TRUE`).
    *   Il messaggio viene parsato: il sistema cerca la `Category` e il `TypedEnum` corrispondenti (tramite nome o ID numerico) per interpretare il payload (Bool, Int o Float).
    *   Viene creato un oggetto `CanbusMessage` che viene poi processato dall'`usbExecutor`, permettendo di simulare comandi CAN o interrogare il nodo direttamente via USB.

#### Gestione Messaggi in Uscita

1.  **Invio su CAN Bus (`sendCanbusMessage`):**
    *   Metodi di alto livello come `sendLog`, `sendEvent` o quelli specifici dei nodi (es. `sendMediaControlMessage`) creano oggetti messaggio specializzati (es. `LogMessage`, `EventMessage`).
    *   `sendCanbusMessage` riceve questi oggetti, ne logga il contenuto per il debug e utilizza la libreria `MCP_CAN` per trasmettere i byte grezzi sul bus.

2.  **Invio su Seriale (`sendSerialMessage`):**
    *   Utilizzato principalmente per riportare lo stato del nodo o rispondere a richieste provenienti dall'interfaccia USB. I messaggi vengono convertiti in stringhe leggibili prima dell'invio.

### Architettura dei Messaggi

Il sistema utilizza un'architettura a classi gerarchica per rappresentare i messaggi scambiati sul bus CAN, garantendo tipizzazione e facilità di parsing.

#### Struttura dell'ID
L'ID di un messaggio CAN (16 bit) è generato dinamicamente combinando due componenti:
*   **Category ID (8 bit):** Definisce il gruppo funzionale (es. `HEARTBEAT`, `SETTING`, `EVENT`).
*   **Enum ID (8 bit):** Identifica il comando o lo stato specifico all'interno di quella categoria.
*   *Esempio:* `id = (category.id << 8) | messageEnum.id`.

#### Gerarchia delle Classi
*   **`CanbusMessage` (Base):** La classe base che contiene l'ID grezzo, il puntatore al payload (`uint8_t*`) e la lunghezza del payload.
*   **Messaggi Specializzati:** Classi che estendono la base per aggiungere logica specifica di formattazione e visualizzazione:
    *   `EventMessage`: Per eventi di sistema (es. `HELLO`, `WARNING`).
    *   `LogMessage`: Per messaggi di log diagnostici tra nodi.
    *   `CarstatusMessage`: Per i dati telemetrici dell'auto (es. giri motore, voltaggio batteria).
    *   `MediaControlMessage`: Specifico per i comandi multimediali.

#### Gestione dei Tipi di Dato (Payload)
Il sistema supporta tre tipi principali di dati nel payload, gestiti tramite `CanbusMessageType`:
*   **BOOL:** Payload di 1 byte (0 o 1).
*   **INT:** Payload di 4 byte (conversione da `int`).
*   **FLOAT:** Payload di 5 byte (un byte di identificazione tipo + 4 byte di dati float).

### Key Libraries and Technologies

*   **`TaskScheduler`:** A cooperative multitasking library used extensively across nodes to manage multiple time-sensitive operations without blocking the main execution loop. It enables periodic task execution, event-driven invocation, and power saving.
    *   **`_TASK_STD_FUNCTION` and `_TASK_SELF_DESTRUCT`:** These preprocessor defines enable support for `std::function` as task callbacks and allow tasks to automatically delete themselves after completion, respectively. This is crucial for flexible and dynamic task management.
*   **`ArduinoOTA`:** Provides the framework for Over-The-Air firmware updates, allowing for remote deployment of new code to the microcontroller.
*   **CAN Bus:** The primary communication protocol for inter-node data exchange, implemented using the `mcp_can` library. Messages are structured using `CanbusMessage` and its derived types (e.g., `EventMessage`, `LogMessage`, `MediaControlMessage`, `CarstatusMessage`).
*   **K-Line (KWP1281):** A diagnostic communication protocol used by `KlineCarduinoNode` to interact with car ECUs, implemented via `SoftwareSerial` and `KLineKWP1281Lib`.
*   **I2C:** Used by `MediaControlCarduinoNode` for communication with the `PCF8574` I/O expander and the `X9C103S` digital potentiometer.
*   **`std::function`:** Utilized extensively for flexible callback binding, particularly with the `TaskScheduler`, `Versatile_RotaryEncoder` event handlers, and K-Line communication functions. This allows for cleaner and more modern C++ code compared to traditional C-style function pointers.
*   **`ArduinoJson`:** Used for parsing and generating JSON data, likely for configuration, API responses, or inter-node communication.
*   **`FunctionalInterrupt`:** Enables the use of `std::function` for interrupt service routines (ISRs), providing more flexibility than traditional C-style function pointers for interrupts.
*   **`ESPAsyncWebServer`:** A non-blocking web server library for ESP microcontrollers, used by `CarduinoNode` to host web interfaces and handle API requests.
*   **`PCF8574`:** An I2C 8-bit I/O expander, used by `MediaControlCarduinoNode` to extend the number of available digital pins for components like the rotary encoder and digital potentiometer.
*   **`Versatile_RotaryEncoder`:** A library for handling rotary encoder inputs, including rotation, press, double press, and long press events.
*   **`X9C103S`:** A digital potentiometer library, used by `MediaControlCarduinoNode` to generate analog resistance values for Steering Wheel Controls.

## Development Practices and Considerations

*   **Modularity:** The project is designed with a strong emphasis on modularity, allowing for independent development and deployment of different functionalities as separate nodes.
*   **Event-Driven Architecture:** The use of callbacks and a task scheduler promotes an event-driven approach, reacting to inputs and time-based triggers efficiently.
*   **Debugging:** Extensive use of `printlnWrapper` and `Serial.println` indicates a focus on logging for debugging purposes during development.
*   **Resource Management:** On embedded systems, careful consideration of dynamic memory allocation and string manipulation is important to prevent memory fragmentation and ensure stability.
*   **TODOs:** The code contains several `TODO` comments, indicating planned features or improvements, such as:
    *   Testing `taskscheduler` for resistance reset.
    *   Refactoring button registration.
    *   Sending messages to the radio during pairing.
    *   Mapping MediaControl to resistance values.
    *   Sending string messages to the radio using IDs.
    *   Creating an enum for the `playTone` function.

## Potential Areas for Improvement (as a Senior Developer)

1.  **Magic Numbers:** Replace literal values (e.g., frequencies, durations in `playTone`) with named constants for improved readability and maintainability.
2.  **Enum for Tones:** Implement the `TODO` to use an `enum` for tone types in `playTone` for better type safety and clarity.
3.  **Consistent Logging:** Standardize the logging mechanism (e.g., exclusively use `printlnWrapper` or a dedicated logging class) for easier control and redirection of output.
4.  **String Efficiency:** Evaluate the use of `String` concatenations in performance-critical or memory-constrained sections. Consider `char[]` buffers or `F()` macro for static strings to reduce dynamic memory overhead.
5.  **Error Handling:** Enhance error handling for K-Line communication (e.g., retries, CAN bus notifications, persistent error logging) beyond simple `Serial.println` messages.
6.  **Watchdog Management:** Confirm the intentional disabling of `heartbeatWdtTask` in `KlineCarduinoNode` and ensure it doesn't compromise system robustness.

This `context.md` serves as a foundational document for understanding the `carduino_arduino` project.