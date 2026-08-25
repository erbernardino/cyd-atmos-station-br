#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "Config.h"

class StorageManager {
public:
    StorageManager();
    void begin();
    
    void loadSettings(AppSettings& settings);
    void saveSettings(const AppSettings& settings);
    
    void addWifiNetwork(AppSettings& settings, const String& ssid, const String& pass);
    void removeWifiNetwork(AppSettings& settings, int index);
    
private:
    Preferences prefs;
};
