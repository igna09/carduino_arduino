#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <map>
#include <functional>

#include "shared/SharedDefinitions.h"
#include "shared/enums/Setting.h"

struct SettingInformation {
    const Setting *setting;
    ValueType *valueType;
    std::function<void(SettingInformation*)> onChange;
    uint8_t address;
};

class SettingBase {
    private:

    public:
        std::map<uint8_t, SettingInformation*> *settings;
        uint8_t nextAddress;    // next address is based on order of addSetting (so if I change order i have to reset values), TODO: base next address on order of setting id

        SettingBase();
        void addSetting(const Setting *setting, bool value, std::function<void(SettingInformation*)> onChange = nullptr);
        void addSetting(const Setting *setting, int value, std::function<void(SettingInformation*)> onChange = nullptr);
        void addSetting(const Setting *setting, float value, std::function<void(SettingInformation*)> onChange = nullptr);
        void putSettingValue(const Setting *setting, bool value);
        void putSettingValue(const Setting *setting, int value);
        void putSettingValue(const Setting *setting, float value);
        SettingInformation* getSettingValue(const Setting *setting);
        void saveSettings();
        void loadSettings();
};
