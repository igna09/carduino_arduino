#pragma once

#include <map>
#include <functional>
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"

#include "Setting.h"
#include "NodeLog.h"
#include "ValueType.h"

struct SettingInformation {
    const Setting *setting;
    ValueType *value;
    ValueType *defaultValue;
    std::function<void(SettingInformation*)> onChange;
    bool doBackup;
};

class SettingBase {
    private:
        // Logger* _logger;
        // FSBase* _fsBase;

    public:
        std::map<uint8_t, SettingInformation*> *settings;
        bool settingsSetupDone;
        bool settingsLoaded;
        bool settingsChanged;

        // SettingBase(Logger*, FSBase*);
        SettingBase();

        void addSetting(const Setting *setting, bool value, std::function<void(SettingInformation*)> onChange = nullptr, bool doBackup = false);
        void addSetting(const Setting *setting, int value, std::function<void(SettingInformation*)> onChange = nullptr, bool doBackup = false);
        void addSetting(const Setting *setting, float value, std::function<void(SettingInformation*)> onChange = nullptr, bool doBackup = false);
        void putSettingValue(const Setting *setting, bool value);
        void putSettingValue(const Setting *setting, int value);
        void putSettingValue(const Setting *setting, float value);
        SettingInformation* getSettingValue(const Setting *setting);
        void backupSettings();
        void restoreSettings();
        void settingsSetup();
        void resetSettings();
};
