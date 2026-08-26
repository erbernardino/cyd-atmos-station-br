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
// 🎨 PALETA DE CORES (Estilo Suíço / Dieter Rams)
// ==========================================
#define SWISS_BG            0x0000  // #000000 - Preto Absoluto
#define SWISS_CARD_BG       0x0842  // #0B0D15 - Cinza Carvão Suave
#define SWISS_BORDER        0x39E7  // #3A3F50 - Linhas Hairline Finas
#define SWISS_TEXT_WHITE    0xFFFF  // #FFFFFF - Branco Puro
#define SWISS_TEXT_MUTED    0x9CD3  // #9AA3B8 - Cinza Médio
#define SWISS_ORANGE        0xFB80  // #FF7000 - Acento Laranja Dieter Rams
#define SWISS_GREEN         0x074A  // #00E676 - Status Bom
#define SWISS_CYAN          0x073F  // #00E5FF - Ciano

// Cores Legado
#define COLOR_GREEN         0x074A
#define COLOR_YELLOW        0xFDE0
#define COLOR_ORANGE        0xFB80
#define COLOR_RED           0xF945
#define COLOR_CYAN          0x073F
#define COLOR_BLUE          0x2CD9

enum ThemeStyle {
    THEME_SWISS = 0,
    THEME_PIXEL,
    THEME_NORDIC,
    THEME_BENTO
};

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
    int theme;
    std::vector<WifiCredential> savedNetworks;
};

// Zona de toque fixa (canto superior direito) que pula direto para PAGE_SETTINGS
#define SETTINGS_TAP_X1     204
#define SETTINGS_TAP_Y1     6
#define SETTINGS_TAP_X2     234
#define SETTINGS_TAP_Y2     30

enum ScreenPage {
    PAGE_NOW = 0,
    PAGE_HOURLY,
    PAGE_WEEK,
    PAGE_AIR,
    PAGE_SETTINGS,
    PAGE_COUNT
};
