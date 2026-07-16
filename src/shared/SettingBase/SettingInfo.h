#pragma once
#include <functional>
#include <cstring>
#include "nvs.h"
#include "Setting.h"
#include "NodeLog.h"

// Interfaccia non-templata per poterle tenere in una singola mappa
struct ISettingInfo {
    virtual ~ISettingInfo() = default;
    virtual uint8_t id() const = 0;
    virtual bool shouldBackup() const = 0;
    virtual void backup(nvs_handle_t h) = 0;
    virtual void restore(nvs_handle_t h) = 0;
    virtual void reset() = 0;
    virtual const Setting* getSetting() const = 0;
    virtual void visit(const std::function<void(const Setting*, int)>& onInt,
                        const std::function<void(const Setting*, float)>& onFloat,
                        const std::function<void(const Setting*, bool)>& onBool) const = 0;
};

template<typename T>
class SettingInfo : public ISettingInfo {
public:
    const Setting *setting;
    T value;
    T defaultValue;
    std::function<void(SettingInfo<T>*)> onChange;
    bool doBackup;

    SettingInfo(const Setting *s, T v, std::function<void(SettingInfo<T>*)> cb, bool backup)
        : setting(s), value(v), defaultValue(v), onChange(cb), doBackup(backup) {}

    uint8_t id() const override { return setting->id; }
    bool shouldBackup() const override { return doBackup; }

    // ritorna true se il valore è cambiato
    bool set(T v) {
        if (v == value) return false;
        value = v;
        if (onChange) onChange(this);
        return true;
    }

    void reset() override { set(defaultValue); }

    void backup(nvs_handle_t h) override;
    void restore(nvs_handle_t h) override;
    
    const Setting* getSetting() const override { return setting; }

    void visit(const std::function<void(const Setting*, int)>& onInt,
               const std::function<void(const Setting*, float)>& onFloat,
               const std::function<void(const Setting*, bool)>& onBool) const override;
};

// --- specializzazioni per tipo, dettano come si parla con l'NVS ---

template<> inline void SettingInfo<bool>::backup(nvs_handle_t h) {
    esp_err_t err = nvs_set_u8(h, setting->name, value ? 1 : 0);
    if (err != ESP_OK) NLOGE("Errore nvs_set_u8 (%s)", setting->name);
}
template<> inline void SettingInfo<bool>::restore(nvs_handle_t h) {
    uint8_t v = 0;
    if (nvs_get_u8(h, setting->name, &v) == ESP_OK) set(v != 0);
}
template<> inline void SettingInfo<bool>::visit(
    const std::function<void(const Setting*, int)>&,
    const std::function<void(const Setting*, float)>&,
    const std::function<void(const Setting*, bool)>& onBool) const {
    onBool(setting, value);
}

template<> inline void SettingInfo<int>::backup(nvs_handle_t h) {
    esp_err_t err = nvs_set_i32(h, setting->name, value);
    if (err != ESP_OK) NLOGE("Errore nvs_set_i32 (%s)", setting->name);
}
template<> inline void SettingInfo<int>::restore(nvs_handle_t h) {
    int32_t v = 0;
    if (nvs_get_i32(h, setting->name, &v) == ESP_OK) set((int)v);
}
template<> inline void SettingInfo<int>::visit(
    const std::function<void(const Setting*, int)>& onInt,
    const std::function<void(const Setting*, float)>&,
    const std::function<void(const Setting*, bool)>&) const {
    onInt(setting, value);
}

template<> inline void SettingInfo<float>::backup(nvs_handle_t h) {
    uint32_t raw;
    memcpy(&raw, &value, sizeof(float));
    esp_err_t err = nvs_set_u32(h, setting->name, raw);
    if (err != ESP_OK) NLOGE("Errore nvs_set_u32 (%s)", setting->name);
}
template<> inline void SettingInfo<float>::restore(nvs_handle_t h) {
    uint32_t raw = 0;
    if (nvs_get_u32(h, setting->name, &raw) == ESP_OK) {
        float v;
        memcpy(&v, &raw, sizeof(float));
        set(v);
    }
}
template<> inline void SettingInfo<float>::visit(
    const std::function<void(const Setting*, int)>&,
    const std::function<void(const Setting*, float)>& onFloat,
    const std::function<void(const Setting*, bool)>&) const {
    onFloat(setting, value);
}