#pragma once

#include <Arduino.h>

// ============================================================================
// LAYOUT DEI PAYLOAD (Max 8 Byte per struct)
// ============================================================================
// Nota: Utilizziamo l'attributo __attribute__((packed)) per impedire al compilatore
// di aggiungere byte di padding (allineamento di memoria), garantendo che la 
// dimensione su RAM corrisponda esattamente ai byte trasmessi sul bus CAN.

/**
 * @brief Layout per EV_SYSTEM_STATUS (Broadcast)
 * Payload totale: 3 Byte
 */
struct PayloadSystemStatus {
    bool uptimeReady;       // 1 Byte
    uint16_t loopTimeMs;    // 2 Byte
} __attribute__((packed));

/**
 * @brief Layout per EV_ENGINE_TELEMETRY (Broadcast)
 * Payload totale: 7 Byte
 */
struct PayloadEngineTelemetry {
    uint16_t rpm;           // 2 Byte
    float coolantTemp;      // 4 Byte
    bool checkEngineLight;  // 1 Byte
} __attribute__((packed));

/**
 * @brief Layout per EV_CLIMATE_STATUS (Broadcast)
 * Payload totale: 5 Byte
 */
struct PayloadClimateStatus {
    float cabinTemp;        // 4 Byte
    uint8_t fanSpeedPercent;// 1 Byte
} __attribute__((packed));

/**
 * @brief Layout per EV_WRITE_SETTING_INT (Point-to-Point)
 * Payload totale: 6 Byte
 */
struct PayloadWriteSettingInt {
    uint16_t settingId;     // 2 Byte
    int32_t newValue;       // 4 Byte
} __attribute__((packed));

/**
 * @brief Layout per EV_TRIGGER_ACTUATOR (Point-to-Point)
 * Payload totale: 2 Byte
 */
struct PayloadTriggerActuator {
    uint8_t pinId;          // 1 Byte
    bool state;             // 1 Byte
} __attribute__((packed));

/**
 * @brief Layout per EV_DIAGNOSTIC_ALERT (Point-to-Point)
 * Payload totale: 5 Byte
 */
struct PayloadDiagnosticAlert {
    uint8_t errorCode;      // 1 Byte
    float componentVoltage; // 4 Byte
} __attribute__((packed));