#include "SettingBase.h"

SettingBase::SettingBase() {
    this->settings = new std::map<uint8_t, SettingInformation*>();
    this->nextAddress = 0;
};

void SettingBase::addSetting(const Setting *setting, bool value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->valueType = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->valueType->boolValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress = this->nextAddress + sizeof(value);
}

void SettingBase::addSetting(const Setting *setting, int value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->valueType = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->valueType->intValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress = this->nextAddress + sizeof(value);
}

void SettingBase::addSetting(const Setting *setting, float value, std::function<void(SettingInformation*)> onChange) {
    SettingInformation *settingInformation = new SettingInformation();
    settingInformation->valueType = new ValueType();
    
    (*this->settings)[setting->id] = settingInformation;

    settingInformation->setting = setting;
    settingInformation->onChange = onChange;
    settingInformation->valueType->floatValue = value;
    settingInformation->address = this->nextAddress;

    this->nextAddress = this->nextAddress + sizeof(value);
}

void SettingBase::putSettingValue(const Setting *setting, bool value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr) {
        settingInformation->valueType->boolValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }
    }
}

void SettingBase::putSettingValue(const Setting *setting, float value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr) {
        settingInformation->valueType->floatValue = value;
        if(settingInformation->onChange != nullptr) {
            settingInformation->onChange(settingInformation);
        }
    }
}

void SettingBase::putSettingValue(const Setting *setting, int value) {
    SettingInformation *settingInformation = getSettingValue(setting);

    if(settingInformation != nullptr) {
        settingInformation->valueType->intValue = value;
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

void SettingBase::saveSettings() {
    std::map<uint8_t, SettingInformation*>::iterator it;

    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        Serial.print(it->second->setting->name);
        Serial.print(" ");
        Serial.println(it->second->address);

        // if(it->second->setting->type->id == CanbusMessageType::BOOL.id) {
        //     EEPROM.put(it->second->address, it->second->valueType->boolValue);
        // } else if(it->second->setting->type->id == CanbusMessageType::INT.id) {
        //     EEPROM.put(it->second->address, it->second->valueType->intValue);
        // } else if(it->second->setting->type->id == CanbusMessageType::FLOAT.id) {
        //     EEPROM.put(it->second->address, it->second->valueType->floatValue);
        // }
    }

    EEPROM.commit();
}

void SettingBase::loadSettings() {
    std::map<uint8_t, SettingInformation*>::iterator it;

    for(it = this->settings->begin(); it != this->settings->end(); it++) {
        Serial.print(it->second->setting->name);
        Serial.print(" ");
        Serial.println(it->second->address);

        // if(it->second->setting->type->id == CanbusMessageType::BOOL.id) {
        //     EEPROM.get(it->second->address, it->second->valueType->boolValue);
        // } else if(it->second->setting->type->id == CanbusMessageType::INT.id) {
        //     EEPROM.get(it->second->address, it->second->valueType->intValue);
        // } else if(it->second->setting->type->id == CanbusMessageType::FLOAT.id) {
        //     EEPROM.get(it->second->address, it->second->valueType->floatValue);
        // }
    }
}
