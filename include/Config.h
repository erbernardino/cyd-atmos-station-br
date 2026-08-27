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
// 🔒 PORTAL WEB - AUTENTICACAO BASICA
// ==========================================
// Credenciais padrao do portal de configuracao (HTTP Basic Auth).
// TROQUE ESTES VALORES antes de expor o dispositivo em rede compartilhada.
#define PORTAL_AUTH_USER    "admin"
#define PORTAL_AUTH_PASS    "atmosbr"

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

enum ThemeStyle {
    THEME_SWISS = 0,
    THEME_PIXEL,
    THEME_NORDIC,
    THEME_BENTO
};
#define THEME_COUNT_IMPLEMENTED 2  // Swiss e Pixel implementados; Nordic/Bento ainda nao

// ==========================================
// 🎮 PALETA DE CORES (Estilo Pixel Art / Retro)
// ==========================================
#define PIXEL_SKY_TOP       0x0885  // #081228 - Azul Noite Escuro
#define PIXEL_SKY_BOTTOM    0x19C9  // #1B3A4B - Teal Profundo
#define PIXEL_BG            0x0885  // #081228 - Fundo solido (paginas sem gradiente)
#define PIXEL_CARD_BG       0x1107  // #10233A - Card Escuro
#define PIXEL_BORDER        0x29C9  // #2C3A4B - Contorno
#define PIXEL_TEXT          0xFF3B  // #FBE7D8 - Creme
#define PIXEL_MUTED         0xADD8  // #A9B8C2 - Cinza Azulado
#define PIXEL_AMBER         0xFB40  // #FF6800 - Acento Amber
#define PIXEL_MOON          0xFF3B  // #FBE7D8 - Creme (lua)
#define PIXEL_CLOUD         0x8CD5  // #8C9AA9 - Nuvem Cinza-Azulada
#define PIXEL_GREEN         0x2731  // #24E68A - Status Bom

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

// Zona de toque do botao "Proximo Estilo" (somente ativo dentro de PAGE_SETTINGS)
#define THEME_TAP_X1        12
#define THEME_TAP_Y1        176
#define THEME_TAP_X2        228
#define THEME_TAP_Y2         208

enum ScreenPage {
    PAGE_NOW = 0,
    PAGE_HOURLY,
    PAGE_WEEK,
    PAGE_AIR,
    PAGE_SETTINGS,
    PAGE_COUNT
};
