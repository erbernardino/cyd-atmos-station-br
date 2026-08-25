#include "DisplayManager.h"
#include "WeatherService.h"

DisplayManager::DisplayManager() : currentBrightness(220) {}

void DisplayManager::init() {
    tft.init();
    tft.setRotation(0); // 240x320 vertical
    tft.fillScreen(SWISS_BG);

    ledcAttach(CYD_TFT_BL, 5000, 8);
    setBrightness(currentBrightness);
}

void DisplayManager::setBrightness(uint8_t brightness) {
    currentBrightness = brightness;
    ledcWrite(CYD_TFT_BL, currentBrightness);
}

String DisplayManager::sanitizeText(const String& str) {
    String result = "";
    result.reserve(str.length());
    for (size_t i = 0; i < str.length(); i++) {
        uint8_t c = (uint8_t)str[i];
        if (c == 0xC3 && i + 1 < str.length()) {
            uint8_t c2 = (uint8_t)str[++i];
            switch (c2) {
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: result += 'A'; break;
                case 0x87: result += 'C'; break;
                case 0x88: case 0x89: case 0x8A: case 0x8B: result += 'E'; break;
                case 0x8C: case 0x8D: case 0x8E: case 0x8F: result += 'I'; break;
                case 0x91: result += 'N'; break;
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: result += 'O'; break;
                case 0x99: case 0x9A: case 0x9B: case 0x9C: result += 'U'; break;
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: result += 'a'; break;
                case 0xA7: result += 'c'; break;
                case 0xA8: case 0xA9: case 0xAA: case 0xAB: result += 'e'; break;
                case 0xAC: case 0xAD: case 0xAE: case 0xAF: result += 'i'; break;
                case 0xB1: result += 'n'; break;
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: result += 'o'; break;
                case 0xB9: case 0xBA: case 0xBB: case 0xBC: result += 'u'; break;
                default: result += ' '; break;
            }
        } else if (c < 128) {
            result += (char)c;
        }
    }
    return result;
}

void DisplayManager::drawHeader(const String& city, const String& timeStr, int wifiRssi, ScreenPage page) {
    // No Estilo Suíço o cabeçalho é integrado harmonicamente na página
}

void DisplayManager::drawLoadingScreen(const String& status) {
    tft.fillScreen(SWISS_BG);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(4);
    tft.drawCentreString("ATMOS BR", 120, 90, 4);

    tft.fillCircle(120, 130, 4, SWISS_ORANGE);

    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString("ESTACAO METEOROLOGICA", 120, 150, 2);
    tft.drawCentreString(sanitizeText(status), 120, 190, 2);
}

// =========================================================================
// 🇨🇭 ESTILO 5: SWISS / DIETER RAMS MINIMALIST (TELA PRINCIPAL)
// =========================================================================
void DisplayManager::drawPageNow(const CurrentWeather& weather, const AirQuality& air) {
    tft.fillScreen(SWISS_BG);

    // 1. TEMPERATURA GIGANTE CENTRALIZADA
    char tempBuf[12];
    snprintf(tempBuf, sizeof(tempBuf), "%.0f", weather.temperature);
    
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(4);
    tft.setTextSize(2); // Fonte grande limpa Helvetica (52px)
    
    int tWidth = tft.textWidth(tempBuf, 4) * 2;
    int startX = (240 - tWidth - 14) / 2;
    tft.drawString(tempBuf, startX, 16, 4);
    
    // Círculo perfeito de grau °
    tft.drawCircle(startX + tWidth + 8, 24, 4, SWISS_TEXT_WHITE);
    tft.drawCircle(startX + tWidth + 8, 24, 3, SWISS_TEXT_WHITE);

    // Reseta escala de texto
    tft.setTextSize(1);

    // 2. ACENTO LARANJA DIETER RAMS
    tft.fillCircle(120, 74, 3, SWISS_ORANGE);

    // 3. CIDADE EM CAIXA ALTA
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    String cityUpper = sanitizeText(weather.lastUpdated.length() > 0 ? "SANTO ANDRE" : "SANTO ANDRE");
    cityUpper.toUpperCase();
    tft.drawCentreString(cityUpper, 120, 84, 2);

    // 4. HORA ATUAL & CONDIÇÃO
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(2);
    char subInfo[32];
    snprintf(subInfo, sizeof(subInfo), "Min %.0f*  Max %.0f*", weather.tempMin, weather.tempMax);
    tft.drawCentreString(subInfo, 120, 106, 2);

    String cond = sanitizeText(weather.weatherDesc);
    cond.toUpperCase();
    tft.setTextColor(SWISS_ORANGE, SWISS_BG);
    tft.setTextFont(1);
    tft.drawCentreString(cond, 120, 126, 1);

    // 5. LINHAS HAIRLINE DO GRID SUÍÇO
    tft.drawFastHLine(12, 142, 216, SWISS_BORDER);
    tft.drawFastHLine(12, 216, 216, SWISS_BORDER);
    tft.drawFastHLine(12, 290, 216, SWISS_BORDER);
    tft.drawFastVLine(120, 142, 148, SWISS_BORDER);

    // ==========================================
    // QUADRANTE 1: VENTO (Top-Left)
    // ==========================================
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("VENTO", 20, 154);
    
    char windBuf[16];
    snprintf(windBuf, sizeof(windBuf), "%.0f KM/H", weather.windSpeed);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(windBuf, 20, 178);

    // ==========================================
    // QUADRANTE 2: UMIDADE (Top-Right)
    // ==========================================
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("UMIDADE", 132, 154);

    char humBuf[10];
    snprintf(humBuf, sizeof(humBuf), "%d %%", weather.humidity);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(humBuf, 132, 178);

    // ==========================================
    // QUADRANTE 3: PRESSÃO (Bottom-Left)
    // ==========================================
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("PRESSAO", 20, 228);

    char pressBuf[16];
    snprintf(pressBuf, sizeof(pressBuf), "%.0f HPA", weather.pressure);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(pressBuf, 20, 252);

    // ==========================================
    // QUADRANTE 4: QUALIDADE DO AR (Bottom-Right)
    // ==========================================
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("QUALIDADE AR", 132, 228);

    String aqiText = sanitizeText(air.levelDesc);
    aqiText.toUpperCase();
    tft.setTextColor(air.levelColor, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(aqiText, 132, 252);

    // 6. BARRA INFERIOR / PAGINAÇÃO
    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 0) {
            tft.fillCircle(dotX + i * 10, 304, 2, SWISS_ORANGE);
        } else {
            tft.drawCircle(dotX + i * 10, 304, 1, SWISS_BORDER);
        }
    }
}

// =========================================================================
// 🇨🇭 TELA 2: PREVISÃO HORÁRIA (SWISS STYLE)
// =========================================================================
void DisplayManager::drawPageHourly(const std::vector<HourlyForecast>& hourly) {
    tft.fillScreen(SWISS_BG);
    
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString("PREVISAO POR HORA", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, SWISS_BORDER);

    int startY = 48;
    for (size_t i = 0; i < hourly.size() && i < 4; i++) {
        int y = startY + i * 58;
        
        char hStr[10];
        snprintf(hStr, sizeof(hStr), "+%dh", hourly[i].hour + 1);
        tft.setTextColor(SWISS_ORANGE, SWISS_BG);
        tft.setTextFont(2);
        tft.drawString(hStr, 20, y + 10);

        char tStr[12];
        snprintf(tStr, sizeof(tStr), "%.0f *C", hourly[i].temperature);
        tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
        tft.setTextFont(2);
        tft.drawString(tStr, 90, y + 10);

        char rStr[16];
        snprintf(rStr, sizeof(rStr), "%d%% chuva", hourly[i].rainProbability);
        tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
        tft.setTextFont(1);
        tft.drawString(rStr, 160, y + 14);

        tft.drawFastHLine(12, y + 46, 216, SWISS_BORDER);
    }

    // Paginação
    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 1) tft.fillCircle(dotX + i * 10, 304, 2, SWISS_ORANGE);
        else tft.drawCircle(dotX + i * 10, 304, 1, SWISS_BORDER);
    }
}

// =========================================================================
// 🇨🇭 TELA 3: PREVISÃO 7 DIAS (SWISS STYLE)
// =========================================================================
void DisplayManager::drawPageWeek(const std::vector<DailyForecast>& daily) {
    tft.fillScreen(SWISS_BG);

    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString("PREVISAO DE 7 DIAS", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, SWISS_BORDER);

    int startY = 46;
    for (size_t i = 0; i < daily.size() && i < 5; i++) {
        int y = startY + i * 48;

        tft.setTextColor(SWISS_ORANGE, SWISS_BG);
        tft.setTextFont(2);
        tft.drawString(daily[i].dayName, 20, y + 8);

        tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
        tft.setTextFont(1);
        tft.drawString(sanitizeText(daily[i].weatherDesc), 85, y + 12);

        char tempRange[20];
        snprintf(tempRange, sizeof(tempRange), "%.0f* / %.0f*", daily[i].tempMin, daily[i].tempMax);
        tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
        tft.setTextFont(2);
        tft.drawRightString(tempRange, 220, y + 8, 2);

        tft.drawFastHLine(12, y + 40, 216, SWISS_BORDER);
    }

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 2) tft.fillCircle(dotX + i * 10, 304, 2, SWISS_ORANGE);
        else tft.drawCircle(dotX + i * 10, 304, 1, SWISS_BORDER);
    }
}

// =========================================================================
// 🇨🇭 TELA 4: QUALIDADE DO AR (SWISS STYLE)
// =========================================================================
void DisplayManager::drawPageAir(const AirQuality& air, const CurrentWeather& current) {
    tft.fillScreen(SWISS_BG);

    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString("QUALIDADE DO AR", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, SWISS_BORDER);

    // AQI Central
    char aqiBuf[16];
    snprintf(aqiBuf, sizeof(aqiBuf), "AQI %d", air.aqi);
    tft.setTextColor(air.levelColor, SWISS_BG);
    tft.setTextFont(4);
    tft.drawCentreString(aqiBuf, 120, 52, 4);

    String desc = sanitizeText(air.levelDesc);
    desc.toUpperCase();
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString(desc, 120, 84, 2);

    tft.drawFastHLine(12, 114, 216, SWISS_BORDER);
    tft.drawFastHLine(12, 194, 216, SWISS_BORDER);
    tft.drawFastHLine(12, 274, 216, SWISS_BORDER);
    tft.drawFastVLine(120, 114, 160, SWISS_BORDER);

    // PM2.5
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("PM 2.5", 20, 126);
    char pm25[16];
    snprintf(pm25, sizeof(pm25), "%.1f ug/m3", air.pm25);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(pm25, 20, 150);

    // PM10
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("PM 10", 132, 126);
    char pm10[16];
    snprintf(pm10, sizeof(pm10), "%.1f ug/m3", air.pm10);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(pm10, 132, 150);

    // OZONIO
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("OZONIO", 20, 206);
    char oz[16];
    snprintf(oz, sizeof(oz), "%.0f ug/m3", air.ozone);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(oz, 20, 230);

    // INDICE UV
    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("INDICE UV", 132, 206);
    char uv[16];
    snprintf(uv, sizeof(uv), "Nivel %d", current.uvIndex);
    tft.setTextColor((current.uvIndex >= 6) ? SWISS_ORANGE : SWISS_GREEN, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(uv, 132, 230);

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 3) tft.fillCircle(dotX + i * 10, 304, 2, SWISS_ORANGE);
        else tft.drawCircle(dotX + i * 10, 304, 1, SWISS_BORDER);
    }
}

// =========================================================================
// 🇨🇭 TELA 5: AJUSTES / STATUS (SWISS STYLE)
// =========================================================================
void DisplayManager::drawPageSettings(const AppSettings& settings, const String& ip) {
    tft.fillScreen(SWISS_BG);

    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawCentreString("STATUS DA ESTACAO", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, SWISS_BORDER);

    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("ENDERECO IP", 20, 52);
    tft.setTextColor(SWISS_ORANGE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(ip, 20, 72);

    tft.drawFastHLine(12, 100, 216, SWISS_BORDER);

    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("CIDADE CONFIGURADA", 20, 114);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(settings.cityName), 20, 134);

    tft.drawFastHLine(12, 162, 216, SWISS_BORDER);

    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("ESTILO VISUAL ATIVO", 20, 176);
    tft.setTextColor(SWISS_TEXT_WHITE, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString("Swiss Minimalist (Dieter Rams)", 20, 196);

    tft.drawFastHLine(12, 224, 216, SWISS_BORDER);

    tft.setTextColor(SWISS_TEXT_MUTED, SWISS_BG);
    tft.setTextFont(1);
    tft.drawString("FIRMWARE", 20, 238);
    tft.setTextColor(SWISS_GREEN, SWISS_BG);
    tft.setTextFont(2);
    tft.drawString("Atmos BR v1.4.0 (Open-Source)", 20, 258);

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 4) tft.fillCircle(dotX + i * 10, 304, 2, SWISS_ORANGE);
        else tft.drawCircle(dotX + i * 10, 304, 1, SWISS_BORDER);
    }
}
