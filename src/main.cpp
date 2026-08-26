#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiManager.h>
#include <time.h>
#include "Config.h"
#include "StorageManager.h"
#include "WeatherService.h"
#include "DisplayManager.h"
#include "TouchHandler.h"
#include "WebPortal.h"

// Instâncias
StorageManager storage;
DisplayManager display;
TouchHandler touch;
WeatherService weatherService;
AppSettings settings;
WebPortal webPortal(settings, storage);
WiFiMulti wifiMulti;

// Estado
CurrentWeather currentWeather;
AirQuality airQuality;
std::vector<HourlyForecast> hourlyForecast;
std::vector<DailyForecast> dailyForecast;

ScreenPage currentPage = PAGE_NOW;
unsigned long lastWeatherUpdate = 0;
unsigned long lastAirUpdate = 0;
unsigned long lastClockMinute = 99;
const unsigned long WEATHER_INTERVAL = 15 * 60 * 1000; // 15 min
const unsigned long AIR_INTERVAL     = 30 * 60 * 1000; // 30 min

void setupRGB() {
    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);
    digitalWrite(CYD_LED_RED, HIGH);
    digitalWrite(CYD_LED_GREEN, HIGH);
    digitalWrite(CYD_LED_BLUE, HIGH);
}

void setRGBColor(bool r, bool g, bool b) {
    if (!settings.rgbLedEnabled) {
        digitalWrite(CYD_LED_RED, HIGH);
        digitalWrite(CYD_LED_GREEN, HIGH);
        digitalWrite(CYD_LED_BLUE, HIGH);
        return;
    }
    digitalWrite(CYD_LED_RED, r ? LOW : HIGH);
    digitalWrite(CYD_LED_GREEN, g ? LOW : HIGH);
    digitalWrite(CYD_LED_BLUE, b ? LOW : HIGH);
}

String getFormattedTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "--:--";
    }
    char buf[10];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
    return String(buf);
}

void refreshCurrentPage() {
    switch (currentPage) {
        case PAGE_NOW:
            display.drawPageNow(currentWeather, airQuality);
            break;
        case PAGE_HOURLY:
            display.drawPageHourly(hourlyForecast);
            break;
        case PAGE_WEEK:
            display.drawPageWeek(dailyForecast);
            break;
        case PAGE_AIR:
            display.drawPageAir(airQuality, currentWeather);
            break;
        case PAGE_SETTINGS:
            display.drawPageSettings(settings, WiFi.localIP().toString());
            break;
        default:
            break;
    }
}

void checkEcoMode() {
    if (!settings.ecoMode) return;
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        int hour = timeinfo.tm_hour;
        bool isEco = false;
        if (settings.ecoStartHour > settings.ecoEndHour) {
            isEco = (hour >= settings.ecoStartHour || hour < settings.ecoEndHour);
        } else {
            isEco = (hour >= settings.ecoStartHour && hour < settings.ecoEndHour);
        }
        display.setBrightness(isEco ? settings.ecoBrightness : settings.brightness);
    }
}

void connectToWiFi() {
    display.drawLoadingScreen("Conectando ao Wi-Fi...");
    
    for (const auto& net : settings.savedNetworks) {
        wifiMulti.addAP(net.ssid.c_str(), net.password.c_str());
        Serial.printf("[Wi-Fi] Registrado: %s\n", net.ssid.c_str());
    }

    bool connected = false;
    if (!settings.savedNetworks.empty()) {
        Serial.println("[Wi-Fi] Tentando conectar as redes...");
        for (int i = 0; i < 15; i++) {
            if (wifiMulti.run() == WL_CONNECTED) {
                connected = true;
                break;
            }
            delay(500);
            Serial.print(".");
        }
    }

    if (!connected) {
        Serial.println("\n[Wi-Fi] Abrindo portal Atmos-Setup...");
        display.drawLoadingScreen("Conecte no Wi-Fi: Atmos-Setup");

        WiFiManager wm;
        wm.setConfigPortalTimeout(180);

        if (!wm.autoConnect("Atmos-Setup", "12345678")) {
            Serial.println("[Wi-Fi] Reiniciando...");
            ESP.restart();
        }

        storage.addWifiNetwork(settings, WiFi.SSID(), WiFi.psk());
    }

    Serial.println("\n[Wi-Fi] Conectado! IP: " + WiFi.localIP().toString());
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 Iniciando Atmos BR - Swiss Minimalist Edition");

    setupRGB();
    setRGBColor(false, false, true);

    storage.begin();
    storage.loadSettings(settings);

    display.init();
    touch.init();
    
    connectToWiFi();
    webPortal.begin();

    display.drawLoadingScreen("Sincronizando relogio NTP...");
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    display.drawLoadingScreen("Obtendo dados meteorologicos...");
    weatherService.updateWeatherData(settings.latitude, settings.longitude, 
                                     currentWeather, hourlyForecast, dailyForecast);
    weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);

    lastWeatherUpdate = millis();
    lastAirUpdate = millis();

    setRGBColor(false, true, false);
    delay(400);
    setRGBColor(false, false, false);

    refreshCurrentPage();
}

void loop() {
    webPortal.handleClient();

    unsigned long now = millis();

    // 1. Atualiza tela a cada minuto (para atualizar o relógio)
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        if (timeinfo.tm_min != lastClockMinute) {
            lastClockMinute = timeinfo.tm_min;
            if (currentPage == PAGE_NOW) {
                refreshCurrentPage();
            }
            checkEcoMode();
        }
    }

    // 2. Atualização Meteorológica (a cada 15 min)
    if (now - lastWeatherUpdate >= WEATHER_INTERVAL) {
        lastWeatherUpdate = now;
        weatherService.updateWeatherData(settings.latitude, settings.longitude, 
                                         currentWeather, hourlyForecast, dailyForecast);
        refreshCurrentPage();
    }

    // 3. Qualidade do Ar (a cada 30 min)
    if (now - lastAirUpdate >= AIR_INTERVAL) {
        lastAirUpdate = now;
        weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);
    }

    // 4. Detecção de Toque
    if (touch.isTouched()) {
        int tx, ty;
        bool gotCoords = touch.getTouchCoordinates(tx, ty);
        bool tappedSettings = gotCoords &&
            tx >= SETTINGS_TAP_X1 && tx <= SETTINGS_TAP_X2 &&
            ty >= SETTINGS_TAP_Y1 && ty <= SETTINGS_TAP_Y2 &&
            currentPage != PAGE_SETTINGS;

        currentPage = tappedSettings ? PAGE_SETTINGS : (ScreenPage)((currentPage + 1) % PAGE_COUNT);
        refreshCurrentPage();
    }

    delay(10);
}
