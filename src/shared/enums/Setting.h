#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define SETTING_SIZE 8

class Setting : public TypedEnum {
    public:
        static const Setting AUTO_CLOSE_REARVIEW_MIRRORS;
        static const Setting OTA_MODE;
        static const Setting ON_REVERSE_LOWER_MIRRORS;
        static const Setting BLE_PAIRING;
        static const Setting BLE_UNLOCKING;
        static const Setting BLE_RSSI_THRESHOLD;
        static const Setting SEND_ALL_MESSAGES_TO_RADIO;
        static const Setting LOG_ON_SERVER;
        // static const Setting* ON_REVERSE_LOWER_MIRRORS;

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
        
        Setting(uint8_t id, const char *name, const CanbusMessageType *type) : TypedEnum(id, name, type) {
            Setting::values[Setting::index] = this;
            Setting::index++;
        };
};

inline const TypedEnum* Setting::values [SETTING_SIZE] = { 0 };
inline uint8_t Setting::index = 0;
inline const Setting Setting::AUTO_CLOSE_REARVIEW_MIRRORS = Setting(0x00, "AUTO_CLOSE_REARVIEW_MIRRORS", &CanbusMessageType::BOOL);
inline const Setting Setting::OTA_MODE = Setting(0x01, "OTA_MODE", &CanbusMessageType::BOOL);
inline const Setting Setting::ON_REVERSE_LOWER_MIRRORS = Setting(0x04, "ON_REVERSE_LOWER_MIRRORS", &CanbusMessageType::BOOL);
inline const Setting Setting::BLE_PAIRING = Setting(0x05, "BLE_PAIRING", &CanbusMessageType::BOOL);
inline const Setting Setting::BLE_UNLOCKING = Setting(0x06, "BLE_UNLOCKING", &CanbusMessageType::BOOL);
inline const Setting Setting::BLE_RSSI_THRESHOLD = Setting(0x07, "BLE_RSSI_THRESHOLD", &CanbusMessageType::INT);
inline const Setting Setting::SEND_ALL_MESSAGES_TO_RADIO = Setting(0x08, "SEND_ALL_MESSAGES_TO_RADIO", &CanbusMessageType::BOOL);
inline const Setting Setting::LOG_ON_SERVER = Setting(0x09, "LOG_ON_SERVER", &CanbusMessageType::BOOL);
// inline const Setting* Setting::ON_REVERSE_LOWER_MIRRORS = new Setting(0x04, "ON_REVERSE_LOWER_MIRRORS", &CanbusMessageType::BOOL);
