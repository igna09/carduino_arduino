#include "SettingBase.h"

// SettingBase::SettingBase(Logger* logger, FSBase* fsBase) {
SettingBase::SettingBase() {
    this->settings = new std::map<uint8_t, SettingInformation*>();
    this->settingsSetupDone = false;
    this->settingsLoaded = false;
    this->settingsChanged = false;
    
    // this->_logger = logger;
    // this->_fsBase = fsBase;

    // 1. Inizializza il partizionamento NVS predefinito
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Se la partizione è corrotta o è cambiato il layout, la cancella e re-inizializza
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
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
    // _logger->printlnWrapper("SettingBase::backupSettings");

    if(!settingsChanged) {
        // _logger->printlnWrapper("SettingBase::backupSettings no changes to save");
        return;
    }

    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }

    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("TAG", "Errore nell'apertura dell'NVS Handle (%s)", esp_err_to_name(err));
        return;
    }

    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->doBackup) {
            err = ESP_OK;
            if(
                settingInformation->setting->type->id == MessageType::BOOL.id
                || settingInformation->setting->type->id == MessageType::BYTE.id
            ) {
                err = nvs_set_u8(my_handle, settingInformation->setting->name, settingInformation->value->boolValue);
            } else if(settingInformation->setting->type->id == MessageType::INT.id) {
                err = nvs_set_i32(my_handle, settingInformation->setting->name, settingInformation->value->intValue);
            } else if(settingInformation->setting->type->id == MessageType::FLOAT.id) {
                uint32_t data_to_save;
                memcpy(&data_to_save, &settingInformation->value->floatValue, sizeof(float)); // Copia i bit senza alterarli
                err = nvs_set_u32(my_handle, settingInformation->setting->name, data_to_save);
            }
            if (err != ESP_OK) ESP_LOGE("TAG", "Errore nella nvs_set!");
        }
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) ESP_LOGE("TAG", "Errore nel commit!");

    nvs_close(my_handle);

    settingsChanged = false;
}

void SettingBase::restoreSettings() {
    // _logger->printlnWrapper("SettingBase::restoreSettings");

    if(!this->settingsSetupDone) {
        this->settingsSetup();
    }

    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("TAG", "Errore nell'apertura dell'NVS Handle (%s)", esp_err_to_name(err));
        return;
    }

    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->doBackup){
            err = ESP_OK;
            if(settingInformation->setting->type->id == MessageType::BOOL.id) {
                uint8_t data_read = 0;
                err = nvs_get_u8(my_handle, settingInformation->setting->name, &data_read);
                bool v = false;
                if (err == ESP_OK) {
                    memcpy(&v, &data_read, sizeof(bool));
                }
                this->putSettingValue(settingInformation->setting, v);
            } else if(settingInformation->setting->type->id == MessageType::BYTE.id) {
                uint8_t v = 0;
                err = nvs_get_u8(my_handle, settingInformation->setting->name, &v);
                this->putSettingValue(settingInformation->setting, v);
            } else if(settingInformation->setting->type->id == MessageType::INT.id) {
                int32_t data_read = 0;
                err = nvs_get_i32(my_handle, settingInformation->setting->name, &data_read);
                int v = 0;
                if (err == ESP_OK) {
                    memcpy(&v, &data_read, sizeof(int));
                }
                this->putSettingValue(settingInformation->setting, v);
            } else if(settingInformation->setting->type->id == MessageType::FLOAT.id) {
                uint32_t data_read;
                err = nvs_get_u32(my_handle, settingInformation->setting->name, &data_read);
                float v = 0.0f;
                if (err == ESP_OK) {
                    memcpy(&v, &data_read, sizeof(float));
                }
                this->putSettingValue(settingInformation->setting, v);
            }
            if (err != ESP_OK) ESP_LOGE("TAG", "Errore nella nvs_set!");
        }
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK) ESP_LOGE("TAG", "Errore nel commit!");

    nvs_close(my_handle);

    this->settingsLoaded = true;
}

void SettingBase::settingsSetup() {
    this->settingsSetupDone = true;
}

void SettingBase::resetSettings() {
    std::map<uint8_t, SettingInformation*>::iterator it;
    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        SettingInformation *settingInformation = it->second;
        if(settingInformation->setting->type->id == MessageType::BOOL.id) {
            settingInformation->value->boolValue = settingInformation->defaultValue->boolValue;
        } else if(settingInformation->setting->type->id == MessageType::INT.id) {
            settingInformation->value->intValue = settingInformation->defaultValue->intValue;
        } else if(settingInformation->setting->type->id == MessageType::FLOAT.id) {
            settingInformation->value->floatValue = settingInformation->defaultValue->floatValue;
        }
    }

    this->backupSettings();
}
