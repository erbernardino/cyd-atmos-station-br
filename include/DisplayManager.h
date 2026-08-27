#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "Config.h"

class DisplayManager {
public:
    DisplayManager();
    void init();
    void setBrightness(uint8_t brightness);
    void setTheme(int theme);

    void drawPageNow(const CurrentWeather& weather, const AirQuality& air);
    void drawPageHourly(const std::vector<HourlyForecast>& hourly);
    void drawPageWeek(const std::vector<DailyForecast>& daily);
    void drawPageAir(const AirQuality& air, const CurrentWeather& current);
    void drawPageSettings(const AppSettings& settings, const String& ip, bool dataStale = false);
    void drawLoadingScreen(const String& status);

    static String sanitizeText(const String& text);
    void drawSettingsButton();

private:
    TFT_eSPI tft;
    uint8_t currentBrightness;
    int activeTheme;

    bool isPixel() const { return activeTheme == THEME_PIXEL; }
    void drawSkyGradient(int y0, int y1, uint16_t top, uint16_t bottom);
    void drawPixelIcon(int x, int y, int weatherCode, bool isDay, int size);
    void drawStat(int x, int labelY, int valueY, const String& label, const String& value, uint16_t valueColor, uint16_t labelColor, uint16_t bg);
};
