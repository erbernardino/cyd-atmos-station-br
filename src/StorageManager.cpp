#include "StorageManager.h"

StorageManager::StorageManager() {}

void StorageManager::begin() {
    prefs.begin("atmos_br", false);
}

void StorageManager::loadSettings(AppSettings& settings) {
    settings.cityName = prefs.getString("city", "Santo Andre");
    settings.latitude = prefs.getFloat("lat", -23.6639f);
    settings.longitude = prefs.getFloat("lon", -46.5383f);
    settings.brightness = prefs.getInt("bright", 220);
    settings.ecoMode = prefs.getBool("eco", true);
    settings.ecoStartHour = prefs.getInt("eco_start", 23);
    settings.ecoEndHour = prefs.getInt("eco_end", 7);
    settings.ecoBrightness = prefs.getInt("eco_br", 40);
    settings.rgbLedEnabled = prefs.getBool("rgb", true);
    settings.theme = prefs.getInt("theme", THEME_SWISS);

    settings.savedNetworks.clear();
    int count = prefs.getInt("wifi_count", 0);
    for (int i = 0; i < count; i++) {
        String sKey = "w_s_" + String(i);
        String pKey = "w_p_" + String(i);
        String s = prefs.getString(sKey.c_str(), "");
        String p = prefs.getString(pKey.c_str(), "");
        if (s.length() > 0) {
            settings.savedNetworks.push_back({.ssid = s, .password = p});
        }
    }

    Serial.printf("[Storage] Configuracoes carregadas. Tema ativo: %d | Redes Wi-Fi: %d\n", 
                  settings.theme, (int)settings.savedNetworks.size());
}

void StorageManager::saveSettings(const AppSettings& settings) {
    prefs.putString("city", settings.cityName);
    prefs.putFloat("lat", settings.latitude);
    prefs.putFloat("lon", settings.longitude);
    prefs.putInt("bright", settings.brightness);
    prefs.putBool("eco", settings.ecoMode);
    prefs.putInt("eco_start", settings.ecoStartHour);
    prefs.putInt("eco_end", settings.ecoEndHour);
    prefs.putInt("eco_br", settings.ecoBrightness);
    prefs.putBool("rgb", settings.rgbLedEnabled);
    prefs.putInt("theme", settings.theme);

    prefs.putInt("wifi_count", (int)settings.savedNetworks.size());
    for (size_t i = 0; i < settings.savedNetworks.size(); i++) {
        String sKey = "w_s_" + String(i);
        String pKey = "w_p_" + String(i);
        prefs.putString(sKey.c_str(), settings.savedNetworks[i].ssid);
        prefs.putString(pKey.c_str(), settings.savedNetworks[i].password);
    }

    Serial.println("[Storage] Configuracoes salvas na memoria NVS.");
}

void StorageManager::addWifiNetwork(AppSettings& settings, const String& ssid, const String& pass) {
    if (ssid.length() == 0) return;
    bool exists = false;
    for (auto& net : settings.savedNetworks) {
        if (net.ssid == ssid) {
            net.password = pass;
            exists = true;
            break;
        }
    }
    if (!exists) {
        settings.savedNetworks.push_back({.ssid = ssid, .password = pass});
    }
    saveSettings(settings);
}

void StorageManager::removeWifiNetwork(AppSettings& settings, int index) {
    if (index >= 0 && index < (int)settings.savedNetworks.size()) {
        settings.savedNetworks.erase(settings.savedNetworks.begin() + index);
        saveSettings(settings);
    }
}
