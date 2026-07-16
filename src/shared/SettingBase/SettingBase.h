#pragma once
#include <map>
#include <memory>
#include "nvs_flash.h"
#include "nvs.h"
#include "SettingInfo.h"
#include "NodeLog.h"

class SettingBase {
public:
    bool settingsChanged = false;
    std::map<uint8_t, std::unique_ptr<ISettingInfo>> settings;

    SettingBase() {
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK(err);
    }

    template<typename T>
    void addSetting(const Setting *setting, T value,
                     std::function<void(SettingInfo<T>*)> onChange = nullptr,
                     bool doBackup = false) {
        settings[setting->id] = std::make_unique<SettingInfo<T>>(setting, value, onChange, doBackup);
    }

    template<typename T>
    SettingInfo<T>* getSetting(const Setting *setting) {
        auto it = settings.find(setting->id);
        if (it == settings.end()) return nullptr;
        return static_cast<SettingInfo<T>*>(it->second.get());
    }

    template<typename T>
    void putSettingValue(const Setting *setting, T value) {
        SettingInfo<T>* info = getSetting<T>(setting);
        if (info && info->set(value)) settingsChanged = true;
    }

    void backupSettings() {
        if (!settingsChanged) return;
        nvs_handle_t h;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &h);
        if (err != ESP_OK) {
            NLOGE("Errore apertura NVS (%s)", esp_err_to_name(err));
            return;
        }
        for (auto &kv : settings) {
            if (kv.second->shouldBackup()) kv.second->backup(h);
        }
        err = nvs_commit(h);
        if (err != ESP_OK) NLOGE("Errore commit NVS!");
        nvs_close(h);
        settingsChanged = false;
    }

    void restoreSettings() {
        nvs_handle_t h;
        esp_err_t err = nvs_open("storage", NVS_READWRITE, &h);
        if (err != ESP_OK) {
            NLOGE("Errore apertura NVS (%s)", esp_err_to_name(err));
            return;
        }
        for (auto &kv : settings) {
            if (kv.second->shouldBackup()) kv.second->restore(h);
        }
        nvs_close(h);
    }

    void resetSettings() {
        for (auto &kv : settings) kv.second->reset();
        backupSettings();
    }
};