#include "SettingBase.h"

SettingBase::SettingBase(Logger* logger, FSBase* fsBase) {
    this->settings = new std::map<uint8_t, SettingInformation*>();
    this->settingsSetupDone = false;
    this->settingsLoaded = false;
    this->settingsChanged = false;
    
    this->_logger = logger;
    this->_fsBase = fsBase;
};

void SettingBase::addSetting(const Setting *setting, bool value, std::function<void(SettingInformation*)> onChange, bool doBackup) {
    // _logger->printlnWrapper("added setting " + String(setting->name), true);
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    settingInformation->value->boolValue = value;
    settingInformation->defaultValue->boolValue = value;
    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->doBackup = doBackup;
    
    (*this->settings)[setting->id] = settingInformation;
}

void SettingBase::addSetting(const Setting *setting, int value, std::function<void(SettingInformation*)> onChange, bool doBackup) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    settingInformation->value->intValue = value;
    settingInformation->defaultValue->intValue = value;
    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->doBackup = doBackup;
    
    (*this->settings)[setting->id] = settingInformation;
}

void SettingBase::addSetting(const Setting *setting, float value, std::function<void(SettingInformation*)> onChange, bool doBackup) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->value = new ValueType();
    settingInformation->defaultValue = new ValueType();
    settingInformation->value->floatValue = value;
    settingInformation->defaultValue->floatValue = value;
    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->doBackup = doBackup;

    (*this->settings)[setting->id] = settingInformation;
}

void SettingBase::putSettingValue(const Setting *setting, bool value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->boolValue) {
        settingInformation->value->boolValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }

        settingsChanged = true;
    }

    // backupSettings();
}

void SettingBase::putSettingValue(const Setting *setting, float value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->floatValue) {
        settingInformation->value->floatValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }

        settingsChanged = true;
    }

    // backupSettings();
}

void SettingBase::putSettingValue(const Setting *setting, int value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr && value != settingInformation->value->intValue) {
        settingInformation->value->intValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }

        settingsChanged = true;
    }

    // backupSettings();
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
    _logger->printlnWrapper("SettingBase::backupSettings");

    if(!settingsChanged) {
        _logger->printlnWrapper("SettingBase::backupSettings no changes to save");
        return;
    }

    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }
    
    JsonDocument settingsJson;

    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->doBackup) {
            // Serial.println(settingInformation->setting->name);
            // String log = String("Backup ") + settingInformation->setting->name + String(" with value ");
            if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
                // log += String(settingInformation->value->boolValue);
                // EEPROM.put(settingInformation->address, settingInformation->value->boolValue);
                settingsJson[settingInformation->setting->name] = settingInformation->value->boolValue;
            } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
                // log += String(settingInformation->value->intValue);
                // EEPROM.put(settingInformation->address, settingInformation->value->intValue);
                settingsJson[settingInformation->setting->name] = settingInformation->value->intValue;
            } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
                // log += String(settingInformation->value->floatValue);
                // EEPROM.put(settingInformation->address, settingInformation->value->floatValue);
                settingsJson[settingInformation->setting->name] = settingInformation->value->floatValue;
            }
            // Serial.println(log);
        }
    }

    File settingsFile = _fsBase->getOrCreateFile("/settings.json", "w");
    serializeJson(settingsJson, settingsFile);
    settingsFile.close();

    settingsChanged = false;

    // EEPROM.commit();
}

void SettingBase::restoreSettings() {
    _logger->printlnWrapper("SettingBase::restoreSettings");
    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }

    if(_fsBase->exists("/settings.json")){
        File settingsFile = _fsBase->getOrCreateFile("/settings.json", "r");
        JsonDocument settingsJson;
        deserializeJson(settingsJson, settingsFile);
        settingsFile.close();
        
        std::map<uint8_t, SettingInformation*>::iterator it;
        for(it = this->settings->begin(); it != this->settings->end(); it++) {
            SettingInformation *settingInformation = it->second;
            if(settingInformation->doBackup){
                String log = String("Restored ") + settingInformation->setting->name + String(" with value ");
                if(settingInformation->setting->type->id == CanbusMessageType::BOOL.id) {
                    bool value = settingsJson[settingInformation->setting->name];
                    // EEPROM.get(settingInformation->address, value);
                    this->putSettingValue(settingInformation->setting, value);
                    log += String(value);
                } else if(settingInformation->setting->type->id == CanbusMessageType::INT.id) {
                    int value = settingsJson[settingInformation->setting->name];
                    // EEPROM.get(settingInformation->address, value);
                    this->putSettingValue(settingInformation->setting, value);
                    log += String(value);
                } else if(settingInformation->setting->type->id == CanbusMessageType::FLOAT.id) {
                    float value = settingsJson[settingInformation->setting->name];
                    // EEPROM.get(settingInformation->address, value);
                    this->putSettingValue(settingInformation->setting, value);
                    log += String(value);
                }
                _logger->printlnWrapper(log);
            }
        }
    }

    this->settingsLoaded = true;
}

void SettingBase::settingsSetup() {
    // EEPROM.begin(this->settingsMemorySize);
    this->settingsSetupDone = true;
}

void SettingBase::resetSettings() {
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
