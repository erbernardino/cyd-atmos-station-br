#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "Config.h"

class DisplayManager {
public:
    DisplayManager();
    void init();
    void setBrightness(uint8_t brightness);

    void drawHeader(const String& city, const String& timeStr, int wifiRssi, ScreenPage page);
    void drawPageNow(const CurrentWeather& weather, const AirQuality& air);
    void drawPageHourly(const std::vector<HourlyForecast>& hourly);
    void drawPageWeek(const std::vector<DailyForecast>& daily);
    void drawPageAir(const AirQuality& air, const CurrentWeather& current);
    void drawPageSettings(const AppSettings& settings, const String& ip);
    void drawLoadingScreen(const String& status);

    void drawCard(int x, int y, int w, int h, const String& title = "");
    void drawWeatherIcon(int x, int y, int weatherCode, bool isDay, int size = 1);

private:
    TFT_eSPI tft;
    uint8_t currentBrightness;
};
