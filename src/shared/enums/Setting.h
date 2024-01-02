#pragma once

#include "TypedEnum.h"
#include "CanbusMessageType.h"

#define SETTING_SIZE 2

class Setting : public TypedEnum {
    public:
        static const Setting AUTO_CLOSE_REARVIEW_MIRRORS;
        static const Setting OTA_MODE;
        static const Setting RESTART;

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
inline const Setting Setting::RESTART = Setting(0x02, "RESTART", &CanbusMessageType::BOOL);
