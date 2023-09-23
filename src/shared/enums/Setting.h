#pragma once

#include "Enum.h"
#include "CanbusMessageType.h"

class Setting : public Enum {
    public:
        static const Setting AUTO_CLOSE_REARVIEW_MIRRORS;
        static const Setting OTA_MODE;
        
        CanbusMessageType type;

        Setting(uint8_t id, const char *name, CanbusMessageType type) : Enum(id, name) {
            this->type = type;
            
            Setting::values[Setting::index] = this;
            Setting::index++;
        };

        static const Enum* getValueById(uint8_t id) {
            for(uint8_t i = 0; i < getSize(); i++) {
                if(Setting::values[i]->id == id) {
                    return Setting::values[i];
                }
            }
            return nullptr;
        }

        static uint8_t getSize() {
            return Setting::index;
        }

        static const Enum** getValues() {
            return Setting::values;
        }

    private:
        static const Enum* values[];
        static uint8_t index;
        
        Setting(uint8_t id, const char *name, const CanbusMessageType *type);
};

//const Enum* Enum::values [] = {&Category::CAR_STATUS, &Category::READ_SETTINGS};
inline const Enum* Setting::values [2] = { 0 };
inline uint8_t Setting::index = 0;
inline const Setting Setting::AUTO_CLOSE_REARVIEW_MIRRORS = Setting(0x00, "AUTO_CLOSE_REARVIEW_MIRRORS", CanbusMessageType::BOOL);
inline const Setting Setting::OTA_MODE = Setting(0x01, "OTA_MODE", CanbusMessageType::BOOL);
