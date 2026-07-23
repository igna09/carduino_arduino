#pragma once

#include "TypedEnum.h"
#include "MessageType.h"

#define SETTING_SIZE 5

class Setting : public TypedEnum {
    public:
        static const Setting TMP_SWC_PRESS_T;
        static const Setting HANDLE_KLINE;
        static const Setting SPD_LMT_LRM;
        static const Setting LOG_SND_RCV_MSG;
        static const Setting OTA_MODE;
        static const Setting LOG_HEAP_STS;
        static const Setting CRUISE_DBG;
        // static const Setting AUTO_CLOSE_REARVIEW_MIRRORS;
        // static const Setting ON_REVERSE_LOWER_MIRRORS;
        // static const Setting BLE_PAIRING;
        // static const Setting BLE_UNLOCKING;
        // static const Setting BLE_RSSI_THRESHOLD;

        static const TypedEnum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Setting::values[i]->id == id) {
                    return Setting::values[i];
                }
            }
            return nullptr;
        }

        static const TypedEnum* getValueByName(char *n) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(strcmp(Setting::values[i]->name, n) == 0) {
                    return Setting::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return Setting::index;
        }

        static const TypedEnum** getValues() {
            return Setting::values;
        }

    private:
        static const TypedEnum* values[];
        static uint8_t index;
        
        Setting(uint8_t id, const char *name, const MessageType *type) : TypedEnum(id, name, type) {
            Setting::values[Setting::index] = this;
            Setting::index++;
        };
};

inline const TypedEnum* Setting::values [SETTING_SIZE] = { 0 };
inline uint8_t Setting::index = 0;
// inline const Setting Setting::AUTO_CLOSE_REARVIEW_MIRRORS = Setting(0x00, "AUTO_CLOSE_REARVIEW_MIRRORS", &MessageType::BOOL);
// inline const Setting Setting::ON_REVERSE_LOWER_MIRRORS = Setting(0x04, "ON_REVERSE_LOWER_MIRRORS", &MessageType::BOOL);
// inline const Setting Setting::BLE_PAIRING = Setting(0x05, "BLE_PAIRING", &MessageType::BOOL);
// inline const Setting Setting::BLE_UNLOCKING = Setting(0x06, "BLE_UNLOCKING", &MessageType::BOOL);
// inline const Setting Setting::BLE_RSSI_THRESHOLD = Setting(0x07, "BLE_RSSI_THRESHOLD", &MessageType::INT);
inline const Setting Setting::TMP_SWC_PRESS_T = Setting(0x00, "TMP_SWC_PRESS_T", &MessageType::INT);
inline const Setting Setting::HANDLE_KLINE = Setting(0x01, "HANDLE_KLINE", &MessageType::BOOL);
inline const Setting Setting::SPD_LMT_LRM = Setting(0x02, "SPD_LMT_LRM", &MessageType::BOOL);
inline const Setting Setting::LOG_SND_RCV_MSG = Setting(0x03, "LOG_SND_RCV_MSG", &MessageType::BOOL);
inline const Setting Setting::OTA_MODE = Setting(0x04, "OTA_MODE", &MessageType::BOOL);
inline const Setting Setting::LOG_HEAP_STS = Setting(0x05, "LOG_HEAP_STS", &MessageType::BOOL);
inline const Setting Setting::CRUISE_DBG = Setting(0x06, "CRUISE_DBG", &MessageType::BOOL);
