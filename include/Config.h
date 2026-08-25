#pragma once

#include <Arduino.h>
#include <vector>

// ==========================================
// 📌 PINAGEM DO HARDWARE - ESP32 CYD
// ==========================================

#define CYD_TFT_BL          21
#define XPT2046_IRQ         36
#define XPT2046_MOSI        32
#define XPT2046_MISO        39
#define XPT2046_CLK         25
#define XPT2046_CS          33

#define CYD_LED_RED         4
#define CYD_LED_GREEN       16
#define CYD_LED_BLUE        17
#define CYD_LDR_PIN         34

// ==========================================
// 🎨 PALETA DE CORES
// ==========================================
#define COLOR_BG            0x0842  // #0B0D15
#define COLOR_CARD_BG       0x18E5  // #171B2B
#define COLOR_CARD_BORDER   0x2969  // #293047
#define COLOR_TEXT_WHITE    0xFFFF  // #FFFFFF
#define COLOR_TEXT_MUTED    0x9CD3  // #9AA3B8
#define COLOR_CYAN          0x073F  // #00E5FF
#define COLOR_ORANGE        0xFB80  // #FF7000
#define COLOR_YELLOW        0xFDE0  // #FFD600
#define COLOR_GREEN         0x074A  // #00E676
#define COLOR_RED           0xF945  // #FF2D55
#define COLOR_BLUE          0x2CD9  // #2979FF

// ==========================================
// 📊 ESTRUTURAS DE DADOS
// ==========================================

struct WifiCredential {
    String ssid;
    String password;
};

struct CurrentWeather {
    float temperature;
    float apparentTemperature;
    float tempMin;
    float tempMax;
    int humidity;
    float pressure;
    float windSpeed;
    float windGusts;
    int windDirection;
    float dewPoint;
    float precipitation;
    float visibilityKm;
    int cloudCover;
    int uvIndex;
    int weatherCode;
    String weatherDesc;
    bool isDay;
    String lastUpdated;
};

struct HourlyForecast {
    int hour;
    float temperature;
    int rainProbability;
    int weatherCode;
};

struct DailyForecast {
    String dayName;
    float tempMax;
    float tempMin;
    int rainProbability;
    int weatherCode;
    String weatherDesc;
};

struct AirQuality {
    int aqi;
    float pm25;
    float pm10;
    float ozone;
    float dust;
    String levelDesc;
    uint16_t levelColor;
};

struct AppSettings {
    String cityName;
    float latitude;
    float longitude;
    int brightness;
    bool ecoMode;
    int ecoStartHour;
    int ecoEndHour;
    int ecoBrightness;
    bool rgbLedEnabled;
    std::vector<WifiCredential> savedNetworks;
};

enum ScreenPage {
    PAGE_NOW = 0,
    PAGE_HOURLY,
    PAGE_WEEK,
    PAGE_AIR,
    PAGE_SETTINGS,
    PAGE_COUNT
};
