#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include "Config.h"
#include "StorageManager.h"

class WebPortal {
public:
    WebPortal(AppSettings& settings, StorageManager& storage);
    void begin();
    void handleClient();

private:
    WebServer server;
    AppSettings& settings;
    StorageManager& storage;

    void handleRoot();
    void handleSaveSettings();
    void handleAddWifi();
    void handleDeleteWifi();
    void handleScanWifi();
    void handleRestart();

    String generateHTML();
};
