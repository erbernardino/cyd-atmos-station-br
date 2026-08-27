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
unsigned long lastWifiCheck = 0;
unsigned long lastWeatherSuccess = 0;
bool weatherDataStale = false;
const unsigned long WEATHER_INTERVAL = 15 * 60 * 1000; // 15 min
const unsigned long AIR_INTERVAL     = 30 * 60 * 1000; // 30 min
const unsigned long WIFI_CHECK_INTERVAL = 30 * 1000;   // 30 s
const unsigned long STALE_THRESHOLD  = 2 * WEATHER_INTERVAL; // dado velho apos 2 ciclos sem sucesso

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
    display.setTheme(settings.theme);
    switch (currentPage) {
        case PAGE_NOW:
            display.drawPageNow(currentWeather, airQuality, settings.cityName, getFormattedTime(), weatherDataStale);
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
            display.drawPageSettings(settings, WiFi.localIP().toString(), WiFi.RSSI(), weatherDataStale);
            break;
        default:
            break;
    }
}

void updateWeatherLED() {
    if (!settings.rgbLedEnabled) {
        setRGBColor(false, false, false);
        return;
    }
    int code = currentWeather.weatherCode;
    if (code >= 95) setRGBColor(true, false, false);              // Tempestade: vermelho
    else if (code >= 51 && code <= 82) setRGBColor(false, false, true); // Chuva: azul
    else if (code >= 45 && code <= 48) setRGBColor(false, true, true);  // Nevoeiro: ciano
    else if (code >= 2 && code <= 3) setRGBColor(true, true, true);     // Nublado: branco
    else if (!currentWeather.isDay) setRGBColor(false, false, true);    // Ceu limpo a noite: azul suave
    else setRGBColor(true, true, false);                                // Ceu limpo de dia: amarelo
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
    if (weatherService.updateWeatherData(settings.latitude, settings.longitude,
                                         currentWeather, hourlyForecast, dailyForecast)) {
        lastWeatherSuccess = millis();
    }
    weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);

    lastWeatherUpdate = millis();
    lastAirUpdate = millis();
    lastWifiCheck = millis();

    setRGBColor(false, true, false);
    delay(400);
    updateWeatherLED();

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
            updateWeatherLED();
        }
    }

    // 2. Atualização Meteorológica (a cada 15 min)
    if (now - lastWeatherUpdate >= WEATHER_INTERVAL) {
        lastWeatherUpdate = now;
        if (weatherService.updateWeatherData(settings.latitude, settings.longitude,
                                             currentWeather, hourlyForecast, dailyForecast)) {
            lastWeatherSuccess = now;
        }
        updateWeatherLED();
        refreshCurrentPage();
    }
    weatherDataStale = (millis() - lastWeatherSuccess) > STALE_THRESHOLD;

    // 3. Qualidade do Ar (a cada 30 min)
    if (now - lastAirUpdate >= AIR_INTERVAL) {
        lastAirUpdate = now;
        weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);
    }

    // 3b. Reconexão automática de Wi-Fi (a cada 30s, se caiu)
    if (now - lastWifiCheck >= WIFI_CHECK_INTERVAL) {
        lastWifiCheck = now;
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[Wi-Fi] Conexao perdida, tentando reconectar...");
            wifiMulti.run();
        }
    }

    // 4. Detecção de Toque
    if (touch.isTouched()) {
        int tx, ty;
        bool gotCoords = touch.getTouchCoordinates(tx, ty);
        bool tappedSettings = gotCoords &&
            tx >= SETTINGS_TAP_X1 && tx <= SETTINGS_TAP_X2 &&
            ty >= SETTINGS_TAP_Y1 && ty <= SETTINGS_TAP_Y2 &&
            currentPage != PAGE_SETTINGS;

        bool tappedTheme = gotCoords && currentPage == PAGE_SETTINGS &&
            tx >= THEME_TAP_X1 && tx <= THEME_TAP_X2 &&
            ty >= THEME_TAP_Y1 && ty <= THEME_TAP_Y2;

        if (gotCoords) {
            display.flashTouch(tx, ty); // feedback visual imediato antes do redraw completo
        }

        if (tappedTheme) {
            settings.theme = (settings.theme + 1) % THEME_COUNT_IMPLEMENTED;
            storage.saveSettings(settings);
        } else {
            currentPage = tappedSettings ? PAGE_SETTINGS : (ScreenPage)((currentPage + 1) % PAGE_COUNT);
        }
        refreshCurrentPage();
    }

    yield();
}
