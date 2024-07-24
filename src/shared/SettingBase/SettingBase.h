#pragma once

#include <Arduino.h>
#include <map>
#include <functional>

#include "shared/SharedDefinitions.h"
#include "shared/enums/Setting.h"
#include "shared/Logger/Logger.h"
#include "shared/FSBase/FSBase.h"

#include <ArduinoJson.h>

struct SettingInformation {
    const Setting *setting;
    ValueType *value;
    ValueType *defaultValue;
    std::function<void(SettingInformation*)> onChange;
    bool doBackup;
};

class SettingBase {
    private:
        Logger* _logger;
        FSBase* _fsBase;

    public:
        std::map<uint8_t, SettingInformation*> *settings;
        bool settingsSetupDone;
        bool settingsLoaded;
        bool settingsChanged;

        SettingBase(Logger*, FSBase*);

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
