#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <map>
#include <functional>
#include "CRC16.h"
#include "CRC.h"

#include "shared/SharedDefinitions.h"
#include "shared/enums/Setting.h"
#include "shared/Logger/Logger.h"

#define CRC_ADDRESS 0
#define CRC_SIZE 2

struct SettingInformation {
    const Setting *setting;
    ValueType *value;
    ValueType *defaultValue;
    std::function<void(SettingInformation*)> onChange;
    uint8_t address;
    bool doBackup;
};

class SettingBase {
    private:
        Logger* _logger;

    public:
        std::map<uint8_t, SettingInformation*> *settings;
        uint8_t nextAddress;    // next address is based on order of addSetting (so if I change order i have to reset values), TODO: base next address on order of setting id
        uint8_t settingsMemorySize;
        bool settingsSetupDone;
        bool settingsLoaded;

        SettingBase();
        void setupSettingBase(Logger*);

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
        uint16_t computeSettingsCrc(std::map<uint8_t, SettingInformation*> *settings);
        void resetEepromSettings();
};
