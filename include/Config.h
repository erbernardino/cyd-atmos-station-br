#pragma once

#include <Arduino.h>

// ==========================================
// 📌 PINAGEM DO HARDWARE - ESP32 CYD
// ==========================================

// Pinos do Display TFT (definidos via build_flags)
#define CYD_TFT_BL          21

// Pinos do Painel Touch (XPT2046 SPI)
#define XPT2046_IRQ         36
#define XPT2046_MOSI        32
#define XPT2046_MISO        39
#define XPT2046_CLK         25
#define XPT2046_CS          33

// Pinos do LED RGB integrado (Lógica Ativa Baixa)
#define CYD_LED_RED         4
#define CYD_LED_GREEN       16
#define CYD_LED_BLUE        17

// Sensor de Luz Ambiente (LDR)
#define CYD_LDR_PIN         34

// ==========================================
// 🎨 PALETA DE CORES (Tema Moderno Escuro)
// ==========================================
#define COLOR_BG            0x0842  // #0B0D15 - Fundo Geral
#define COLOR_CARD_BG       0x18E5  // #171B2B - Fundo dos Cards
#define COLOR_CARD_BORDER   0x2969  // #293047 - Borda dos Cards
#define COLOR_TEXT_WHITE    0xFFFF  // #FFFFFF - Texto Principal
#define COLOR_TEXT_MUTED    0x9CD3  // #9AA3B8 - Texto Secundário
#define COLOR_CYAN          0x073F  // #00E5FF - Destaques
#define COLOR_ORANGE        0xFB80  // #FF7000 - Temperatura Alta
#define COLOR_YELLOW        0xFDE0  // #FFD600 - Alertas
#define COLOR_GREEN         0x074A  // #00E676 - Status Bom
#define COLOR_RED           0xF945  // #FF2D55 - Alertas Críticos
#define COLOR_BLUE          0x2CD9  // #2979FF - Chuva / Vento

// ==========================================
// 📊 ESTRUTURAS DE DADOS METEOROLÓGICOS
// ==========================================

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
};

enum ScreenPage {
    PAGE_NOW = 0,
    PAGE_HOURLY,
    PAGE_WEEK,
    PAGE_AIR,
    PAGE_SETTINGS,
    PAGE_COUNT
};
