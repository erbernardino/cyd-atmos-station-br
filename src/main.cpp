#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <time.h>
#include "Config.h"
#include "WeatherService.h"
#include "DisplayManager.h"
#include "TouchHandler.h"

// Instâncias dos Serviços
DisplayManager display;
TouchHandler touch;
WeatherService weatherService;

// Estado da Aplicação
CurrentWeather currentWeather;
AirQuality airQuality;
std::vector<HourlyForecast> hourlyForecast;
std::vector<DailyForecast> dailyForecast;

AppSettings settings = {
    .cityName = "Sao Paulo",
    .latitude = -23.5505f,
    .longitude = -46.6333f,
    .brightness = 220,
    .ecoMode = true,
    .ecoStartHour = 23,
    .ecoEndHour = 7,
    .ecoBrightness = 40,
    .rgbLedEnabled = true
};

ScreenPage currentPage = PAGE_NOW;
unsigned long lastWeatherUpdate = 0;
unsigned long lastAirUpdate = 0;
unsigned long lastClockUpdate = 0;
const unsigned long WEATHER_INTERVAL = 15 * 60 * 1000; // 15 minutos
const unsigned long AIR_INTERVAL     = 30 * 60 * 1000; // 30 minutos

void setupRGB() {
    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);
    // Desliga todos inicialmente (Lógica Ativa Baixa)
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
    display.drawHeader(settings.cityName, getFormattedTime(), WiFi.RSSI(), currentPage);
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

void setup() {
    Serial.begin(115200);
    Serial.println("\n🚀 Iniciando Atmos BR - Estacao Meteorologica CYD");

    setupRGB();
    setRGBColor(false, false, true); // Azul durante boot

    display.init();
    touch.init();
    display.drawLoadingScreen("Conectando ao Wi-Fi...");

    // Gerenciador de Wi-Fi Inteligente com Portal Cativo
    WiFiManager wm;
    wm.setConfigPortalTimeout(180);

    if (!wm.autoConnect("Atmos-Setup", "12345678")) {
        Serial.println("[Wi-Fi] Falha na conexao. Reiniciando...");
        display.drawLoadingScreen("Falha no Wi-Fi. Reiniciando...");
        delay(3000);
        ESP.restart();
    }

    Serial.println("[Wi-Fi] Conectado! IP: " + WiFi.localIP().toString());
    display.drawLoadingScreen("Sincronizando relogio NTP...");

    // Fuso Horário de Brasília (UTC-3)
    configTime(-3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    display.drawLoadingScreen("Obtendo previsao do tempo...");
    weatherService.updateWeatherData(settings.latitude, settings.longitude, 
                                     currentWeather, hourlyForecast, dailyForecast);
    weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);

    lastWeatherUpdate = millis();
    lastAirUpdate = millis();

    setRGBColor(false, true, false); // Verde: Conectado e operando
    delay(500);
    setRGBColor(false, false, false); // Apaga para não incomodar

    refreshCurrentPage();
}

void loop() {
    unsigned long now = millis();

    // 1. Atualização do Relógio (a cada segundo)
    if (now - lastClockUpdate >= 1000) {
        lastClockUpdate = now;
        display.drawHeader(settings.cityName, getFormattedTime(), WiFi.RSSI(), currentPage);
        checkEcoMode();
    }

    // 2. Atualização Meteorológica (a cada 15 min)
    if (now - lastWeatherUpdate >= WEATHER_INTERVAL) {
        lastWeatherUpdate = now;
        Serial.println("[Loop] Atualizando clima...");
        weatherService.updateWeatherData(settings.latitude, settings.longitude, 
                                         currentWeather, hourlyForecast, dailyForecast);
        refreshCurrentPage();
    }

    // 3. Atualização da Qualidade do Ar (a cada 30 min)
    if (now - lastAirUpdate >= AIR_INTERVAL) {
        lastAirUpdate = now;
        weatherService.updateAirQuality(settings.latitude, settings.longitude, airQuality);
    }

    // 4. Detecção de Toque na Tela Touch
    if (touch.isTouched()) {
        currentPage = (ScreenPage)((currentPage + 1) % PAGE_COUNT);
        Serial.printf("[Touch] Mudando para a pagina %d\n", (int)currentPage);
        refreshCurrentPage();
    }

    delay(20);
}
