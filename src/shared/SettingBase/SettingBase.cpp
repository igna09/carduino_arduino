#include "SettingBase.h"

SettingBase::SettingBase() {
    this->settings = new std::map<uint8_t, SettingInformation*>();
    this->nextAddress = CRC_ADDRESS + CRC_SIZE;
    this->settingsMemorySize = CRC_SIZE;
    this->settingsSetupDone = false;
    this->settingsLoaded = false;
};

void SettingBase::addSetting(const Setting *setting, bool value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->value->boolValue = value;
    settingInformation->defaultValue->boolValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress += sizeof(value);
    this->settingsMemorySize += sizeof(value);
}

void SettingBase::addSetting(const Setting *setting, int value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->value->intValue = value;
    settingInformation->defaultValue->intValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress += sizeof(value);
    this->settingsMemorySize += sizeof(value);
}

void SettingBase::addSetting(const Setting *setting, float value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->value->floatValue = value;
    settingInformation->defaultValue->floatValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress += sizeof(value);
    this->settingsMemorySize += sizeof(value);
}

void SettingBase::putSettingValue(const Setting *setting, bool value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->boolValue) {
        settingInformation->value->boolValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }
    }
}

void SettingBase::putSettingValue(const Setting *setting, float value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->floatValue) {
        settingInformation->value->floatValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }
    }
}

void SettingBase::putSettingValue(const Setting *setting, int value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->intValue) {
        settingInformation->value->intValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }
    }
}

SettingInformation* SettingBase::getSettingValue(const Setting *setting) {
    std::map<uint8_t, SettingInformation*>::iterator it = this->settings->find(setting->id);

    if(it == this->settings->end()) {
        return nullptr;
    } else {
        return it->second;
    }
}

void SettingBase::backupSettings() {
    // Serial.println("SettingBase::backupSettings");
    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }

    /**
     * compute CRC from RAM settings
    */
    uint16_t computeSettingsCrc = this->computeSettingsCrc(this->settings);
    EEPROM.put(CRC_ADDRESS, computeSettingsCrc);

    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        // String log = String("Backup ") + settingInformation->setting->name + String(" with value ");
        if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
            // log += String(settingInformation->value->boolValue);
            EEPROM.put(settingInformation->address, settingInformation->value->boolValue);
        } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
            // log += String(settingInformation->value->intValue);
            EEPROM.put(settingInformation->address, settingInformation->value->intValue);
        } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
            // log += String(settingInformation->value->floatValue);
            EEPROM.put(settingInformation->address, settingInformation->value->floatValue);
        }
        // Serial.println(log);
    }

    EEPROM.commit();
}

void SettingBase::restoreSettings() {
    // Serial.println("SettingBase::restoreSettings");
    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }

    /**
     * compute CRC from EEPROM settings
    */
    std::map<uint8_t, SettingInformation*>::iterator it;
    CRC16 crc;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
            bool value;
            EEPROM.get(settingInformation->address, value);
            crc.add(value);
        } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
            int value;
            EEPROM.get(settingInformation->address, value);
            crc.add(value);
        } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
            float value;
            EEPROM.get(settingInformation->address, value);
            crc.add(value);
        }
    }
    uint16_t crcCalculatedFromEepromSettings = crc.calc();

    /**
     * get CRC stored on EEPROM
    */
    uint16_t crcFromEeprom;
    EEPROM.get(CRC_ADDRESS, crcFromEeprom);

    if(crcCalculatedFromEepromSettings == crcFromEeprom) {
        for(it = this->settings->begin(); it != this->settings->end(); it++) {
            SettingInformation *settingInformation = it->second;
            // String log = String("Restored ") + settingInformation->setting->name + String(" with value ");
            if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
                bool value;
                EEPROM.get(settingInformation->address, value);
                this->putSettingValue(settingInformation->setting, value);
                // log += String(value);
            } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
                int value;
                EEPROM.get(settingInformation->address, value);
                this->putSettingValue(settingInformation->setting, value);
                // log += String(value);
            } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
                float value;
                EEPROM.get(settingInformation->address, value);
                this->putSettingValue(settingInformation->setting, value);
                // log += String(value);
            }
            // Serial.println(log);
        }
    }

    this->settingsLoaded = true;
}

void SettingBase::settingsSetup() {
    EEPROM.begin(this->settingsMemorySize);
    this->settingsSetupDone = true;
}

uint16_t SettingBase::computeSettingsCrc(std::map<uint8_t, SettingInformation*> *settings) {
    std::map<uint8_t, SettingInformation*>::iterator it;
    CRC16 crc;

    for(it = settings->begin(); it != settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
            crc.add(settingInformation->value->boolValue);
        } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
            crc.add(settingInformation->value->intValue);
        } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
            crc.add(settingInformation->value->floatValue);
        }
    }
    return crc.calc();
}

void SettingBase::resetEepromSettings() {
    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
            settingInformation->value->boolValue = settingInformation->defaultValue->boolValue;
        } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
            settingInformation->value->intValue = settingInformation->defaultValue->intValue;
        } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
            settingInformation->value->floatValue = settingInformation->defaultValue->floatValue;
        }
    }

    this->backupSettings();
}
