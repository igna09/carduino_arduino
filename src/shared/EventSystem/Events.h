#pragma once

// ---------------------------------------------------------------------------
// Events.h
//   Unico file dove si dichiarano tutti gli eventi.
//   Aggiungere un evento = aggiungere una riga qui, nient'altro.
//
//   Sintassi: DECLARE_EVENT(NomeVariabile, id, "NOME_STRINGA", Tipo1, Tipo2, ...)
//   Payload vuoto: omettere i tipi (evento senza dati)
// ---------------------------------------------------------------------------

#include "EventRegistry.h"

// --- Controllo / lifecycle --------------------------------------------------
DECLARE_EVENT(ENABLE,                   0x00, "ENABLE")
DECLARE_EVENT(ENABLE_INTERRUPT,         0x01, "ENABLE_INTERRUPT")
DECLARE_EVENT(ENABLE_FINISH,            0x02, "ENABLE_FINISH")
DECLARE_EVENT(DISABLE,                  0x03, "DISABLE")
DECLARE_EVENT(DISABLE_INTERRUPT,        0x04, "DISABLE_INTERRUPT")
DECLARE_EVENT(DISABLE_FINISH,           0x05, "DISABLE_FINISH")
DECLARE_EVENT(GET_HELLOS,               0x06, "GET_HELLOS")
DECLARE_EVENT(HELLO,                    0x07, "HELLO",                      uint8_t)
DECLARE_EVENT(HEARTBEAT,                0x08, "HEARTBEAT")
DECLARE_EVENT(LOCK_CAR,                 0x09, "LOCK_CAR")
DECLARE_EVENT(UNLOCK_CAR,               0x0A, "UNLOCK_CAR")
DECLARE_EVENT(TEST,                     0x0B, "TEST")
DECLARE_EVENT(RESET_WEBAPP,             0x0C, "RESET_WEBAPP")
DECLARE_EVENT(BLE_PAIRING_CODE,         0x0D, "BLE_PAIRING_CODE")
DECLARE_EVENT(RESTART,                  0x0E, "RESTART")
DECLARE_EVENT(SWC_PAIR,                 0x0F, "SWC_PAIR")
DECLARE_EVENT(GET_SETTINGS,             0x10, "GET_SETTINGS")
DECLARE_EVENT(WARNING_SEVERITY_LOW,     0x11, "WARNING_SEVERITY_LOW")
DECLARE_EVENT(WARNING_SEVERITY_MEDIUM,  0x12, "WARNING_SEVERITY_MEDIUM")
DECLARE_EVENT(WARNING_SEVERITY_HIGH,    0x13, "WARNING_SEVERITY_HIGH")

// --- Sensori ----------------------------------------------------------------
DECLARE_EVENT(EXTERNAL_TEMPERATURE,             0x14, "EXTERNAL_TEMPERATURE",             float)
DECLARE_EVENT(INTERNAL_TEMPERATURE,             0x15, "INTERNAL_TEMPERATURE",             float)
DECLARE_EVENT(SPEED,                            0x16, "SPEED",                            uint32_t) //uint8_t
DECLARE_EVENT(INTERNAL_LUMINANCE,               0x17, "INTERNAL_LUMINANCE",               uint16_t)
DECLARE_EVENT(FRONT_DISTANCE,                   0x18, "FRONT_DISTANCE",                   float)
DECLARE_EVENT(ENGINE_WATER_COOLING_TEMPERATURE, 0x19, "ENGINE_WATER_COOLING_TEMPERATURE", float)
DECLARE_EVENT(ENGINE_OIL_TEMPERATURE,           0x1A, "ENGINE_OIL_TEMPERATURE",           float)
DECLARE_EVENT(ENGINE_INTAKE_MANIFOLD_PRESSURE,  0x1B, "ENGINE_INTAKE_MANIFOLD_PRESSURE",  float)
DECLARE_EVENT(ENGINE_RPM,                       0x1C, "ENGINE_RPM",                       uint32_t) //uint16_t
DECLARE_EVENT(TRIP_DURATION,                    0x1D, "TRIP_DURATION",                    uint32_t)
DECLARE_EVENT(TRIP_AVERAGE_SPEED,               0x1E, "TRIP_AVERAGE_SPEED",               uint8_t)
DECLARE_EVENT(TRIP_MAX_SPEED,                   0x1F, "TRIP_MAX_SPEED",                   uint8_t)
DECLARE_EVENT(INJECTED_QUANTITY,                0x20, "INJECTED_QUANTITY",                float)
DECLARE_EVENT(FUEL_CONSUMPTION,                 0x21, "FUEL_CONSUMPTION",                 float)
DECLARE_EVENT(BATTERY_VOLTAGE,                  0x22, "BATTERY_VOLTAGE",                  float)
DECLARE_EVENT(IS_REVERSE,                       0x23, "IS_REVERSE",                       bool)
DECLARE_EVENT(IS_KEY_ON,                        0x24, "IS_KEY_ON",                        bool)

// --- Controllo media --------------------------------------------------------
DECLARE_EVENT(VOLUME_UP,    0x25, "VOLUME_UP")
DECLARE_EVENT(VOLUME_DOWN,  0x26, "VOLUME_DOWN")
DECLARE_EVENT(PLAY_PAUSE,   0x27, "PLAY_PAUSE")
DECLARE_EVENT(NEXT,         0x28, "NEXT")
DECLARE_EVENT(LONG_PRESS,   0x29, "LONG_PRESS")

// --- Sincronizzazione tempo -------------------------------------------------
// Payload TIME_SYNC_REQUEST: [source node id (1B)]
DECLARE_EVENT(TIME_SYNC_REQUEST,  0x2A, "TIME_SYNC_REQUEST", uint8_t)
// Payload TIME_SYNC_RESPONSE: [T2 ricezione request su MAIN (4B)] + [T3 invio response da MAIN (4B)], entrambi in ms (esp_timer_get_time()/1000)
DECLARE_EVENT(TIME_SYNC_RESPONSE, 0x2B, "TIME_SYNC_RESPONSE", uint32_t, uint32_t)

// --- Impostazioni -----------------------------------------------------------
// Payload: [Setting ID (1B)] + [Value (4B)] = 5 byte totali
DECLARE_EVENT(WRITE_SETTING, 0x2C, "WRITE_SETTING", uint8_t, int32_t)
DECLARE_EVENT(READ_SETTING,  0x2D, "READ_SETTING",  uint8_t, int32_t)