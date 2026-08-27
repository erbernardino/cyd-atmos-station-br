#include "DisplayManager.h"
#include "WeatherService.h"
#include "PixelIcons.h"

DisplayManager::DisplayManager() : currentBrightness(220), activeTheme(THEME_SWISS) {}

void DisplayManager::setTheme(int theme) {
    activeTheme = theme;
}

void DisplayManager::drawSkyGradient(int y0, int y1, uint16_t top, uint16_t bottom) {
    uint8_t tr = (top >> 11) & 0x1F, tg = (top >> 5) & 0x3F, tb = top & 0x1F;
    uint8_t br = (bottom >> 11) & 0x1F, bg = (bottom >> 5) & 0x3F, bb = bottom & 0x1F;
    int h = y1 - y0;
    for (int y = 0; y < h; y++) {
        float t = (float)y / (float)h;
        uint8_t r = tr + (br - tr) * t;
        uint8_t g = tg + (bg - tg) * t;
        uint8_t b = tb + (bb - tb) * t;
        uint16_t color = (r << 11) | (g << 5) | b;
        tft.drawFastHLine(0, y0 + y, 240, color);
    }
}

void DisplayManager::drawPixelIcon(int x, int y, int weatherCode, bool isDay, int size) {
    // Sprites 16x16 desenhados a partir do canto superior esquerdo (x,y = topo-esquerda do icone)
    const uint8_t* bitmap;
    uint16_t color;

    if (weatherCode == 0 || weatherCode == 1) {
        bitmap = isDay ? ICON_SUN : ICON_MOON;
        color = isDay ? PIXEL_AMBER : PIXEL_MOON;
    } else if (weatherCode >= 45 && weatherCode <= 48) {
        bitmap = ICON_FOG;
        color = PIXEL_CLOUD;
    } else if (weatherCode >= 51 && weatherCode <= 82) {
        bitmap = ICON_RAIN;
        color = PIXEL_CLOUD;
    } else if (weatherCode >= 95) {
        bitmap = ICON_STORM;
        color = PIXEL_CLOUD;
    } else {
        bitmap = ICON_CLOUD;
        color = PIXEL_CLOUD;
    }

    // x,y = centro do icone (mantém compatibilidade com os pontos de chamada existentes)
    tft.drawBitmap(x - 8, y - 8, bitmap, 16, 16, color);
}

void DisplayManager::init() {
    tft.init();
    tft.setRotation(0); // 240x320 vertical
    
    // CORREÇÃO CRÍTICA DO PAINEL CYD: Inverte cores para Preto Real (#000000) e Cores Fiéis
    tft.invertDisplay(true);
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

void DisplayManager::drawStat(int x, int labelY, int valueY, const String& label, const String& value, uint16_t valueColor, uint16_t labelColor, uint16_t bg) {
    tft.setTextColor(labelColor, bg);
    tft.setTextFont(1);
    tft.drawString(label, x, labelY);
    tft.setTextColor(valueColor, bg);
    tft.setTextFont(2);
    tft.drawString(value, x, valueY);
}

void DisplayManager::drawSettingsButton() {
    int cx = (SETTINGS_TAP_X1 + SETTINGS_TAP_X2) / 2;
    int cy = (SETTINGS_TAP_Y1 + SETTINGS_TAP_Y2) / 2;
    tft.fillCircle(cx, cy - 5, 1, SWISS_TEXT_MUTED);
    tft.fillCircle(cx, cy, 1, SWISS_TEXT_MUTED);
    tft.fillCircle(cx, cy + 5, 1, SWISS_TEXT_MUTED);
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

void DisplayManager::drawPageNow(const CurrentWeather& weather, const AirQuality& air) {
    uint16_t bg = isPixel() ? PIXEL_CARD_BG : SWISS_BG;
    uint16_t textW = isPixel() ? PIXEL_TEXT : SWISS_TEXT_WHITE;
    uint16_t textM = isPixel() ? PIXEL_MUTED : SWISS_TEXT_MUTED;
    uint16_t accent = isPixel() ? PIXEL_AMBER : SWISS_ORANGE;
    uint16_t border = isPixel() ? PIXEL_BORDER : SWISS_BORDER;

    if (isPixel()) {
        drawSkyGradient(0, 142, PIXEL_SKY_TOP, PIXEL_SKY_BOTTOM);
        tft.fillRect(0, 142, 240, 178, PIXEL_CARD_BG);
        drawPixelIcon(200, 30, weather.weatherCode, weather.isDay, 3);
    } else {
        tft.fillScreen(bg);
    }

    // 1. TEMPERATURA GIGANTE CENTRALIZADA
    char tempBuf[12];
    snprintf(tempBuf, sizeof(tempBuf), "%.0f", weather.temperature);

    tft.setTextColor(textW, bg);
    tft.setTextFont(4);
    tft.setTextSize(2);

    int tWidth = tft.textWidth(tempBuf, 4) * 2;
    int startX = (240 - tWidth - 14) / 2;
    tft.drawString(tempBuf, startX, 16, 4);

    // Círculo perfeito de grau °
    tft.drawCircle(startX + tWidth + 8, 24, 4, textW);
    tft.drawCircle(startX + tWidth + 8, 24, 3, textW);

    tft.setTextSize(1);
    drawSettingsButton();

    // 2. ACENTO
    if (!isPixel()) tft.fillCircle(120, 74, 3, accent);

    // 3. CIDADE EM CAIXA ALTA
    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    String cityUpper = sanitizeText(weather.lastUpdated.length() > 0 ? "SANTO ANDRE" : "SANTO ANDRE");
    cityUpper.toUpperCase();
    tft.drawCentreString(cityUpper, 120, 84, 2);

    // 4. HORA ATUAL & FAIXA DE TEMPERATURA
    tft.setTextColor(textM, bg);
    tft.setTextFont(2);
    char subInfo[32];
    snprintf(subInfo, sizeof(subInfo), "Min %.0f*  Max %.0f*", weather.tempMin, weather.tempMax);
    tft.drawCentreString(subInfo, 120, 106, 2);

    String cond = sanitizeText(weather.weatherDesc);
    cond.toUpperCase();
    tft.setTextColor(accent, bg);
    tft.setTextFont(1);
    tft.drawCentreString(cond, 120, 126, 1);

    // 5. LINHAS HAIRLINE DO GRID
    tft.drawFastHLine(12, 142, 216, border);
    tft.drawFastHLine(12, 216, 216, border);
    tft.drawFastHLine(12, 290, 216, border);
    tft.drawFastVLine(120, 142, 148, border);

    // QUADRANTE 1: VENTO
    char windBuf[16];
    snprintf(windBuf, sizeof(windBuf), "%.0f KM/H", weather.windSpeed);
    drawStat(20, 154, 178, "VENTO", windBuf, textW, textM, bg);

    // QUADRANTE 2: UMIDADE
    char humBuf[10];
    snprintf(humBuf, sizeof(humBuf), "%d %%", weather.humidity);
    drawStat(132, 154, 178, "UMIDADE", humBuf, textW, textM, bg);

    // QUADRANTE 3: PRESSÃO
    char pressBuf[16];
    snprintf(pressBuf, sizeof(pressBuf), "%.0f HPA", weather.pressure);
    drawStat(20, 228, 252, "PRESSAO", pressBuf, textW, textM, bg);

    // QUADRANTE 4: QUALIDADE DO AR
    String aqiText = sanitizeText(air.levelDesc);
    aqiText.toUpperCase();
    drawStat(132, 228, 252, "QUALIDADE AR", aqiText, air.levelColor, textM, bg);

    // BARRA INFERIOR / PAGINAÇÃO
    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 0) tft.fillCircle(dotX + i * 10, 304, 2, accent);
        else tft.drawCircle(dotX + i * 10, 304, 1, border);
    }
}

void DisplayManager::drawPageHourly(const std::vector<HourlyForecast>& hourly) {
    uint16_t bg = isPixel() ? PIXEL_BG : SWISS_BG;
    uint16_t textW = isPixel() ? PIXEL_TEXT : SWISS_TEXT_WHITE;
    uint16_t textM = isPixel() ? PIXEL_MUTED : SWISS_TEXT_MUTED;
    uint16_t accent = isPixel() ? PIXEL_AMBER : SWISS_ORANGE;
    uint16_t border = isPixel() ? PIXEL_BORDER : SWISS_BORDER;

    tft.fillScreen(bg);

    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    tft.drawCentreString("PREVISAO POR HORA", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, border);
    drawSettingsButton();

    int startY = 48;
    for (size_t i = 0; i < hourly.size() && i < 4; i++) {
        int y = startY + i * 58;

        char hStr[10];
        snprintf(hStr, sizeof(hStr), "+%dh", hourly[i].hour + 1);
        tft.setTextColor(accent, bg);
        tft.setTextFont(2);
        tft.drawString(hStr, 20, y + 10);

        if (isPixel()) drawPixelIcon(60, y + 14, hourly[i].weatherCode, true, 1);

        char tStr[12];
        snprintf(tStr, sizeof(tStr), "%.0f *C", hourly[i].temperature);
        tft.setTextColor(textW, bg);
        tft.setTextFont(2);
        tft.drawString(tStr, 90, y + 10);

        char rStr[16];
        snprintf(rStr, sizeof(rStr), "%d%% chuva", hourly[i].rainProbability);
        tft.setTextColor(textM, bg);
        tft.setTextFont(1);
        tft.drawString(rStr, 160, y + 14);

        tft.drawFastHLine(12, y + 46, 216, border);
    }

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 1) tft.fillCircle(dotX + i * 10, 304, 2, accent);
        else tft.drawCircle(dotX + i * 10, 304, 1, border);
    }
}

void DisplayManager::drawPageWeek(const std::vector<DailyForecast>& daily) {
    uint16_t bg = isPixel() ? PIXEL_BG : SWISS_BG;
    uint16_t textW = isPixel() ? PIXEL_TEXT : SWISS_TEXT_WHITE;
    uint16_t textM = isPixel() ? PIXEL_MUTED : SWISS_TEXT_MUTED;
    uint16_t accent = isPixel() ? PIXEL_AMBER : SWISS_ORANGE;
    uint16_t border = isPixel() ? PIXEL_BORDER : SWISS_BORDER;

    tft.fillScreen(bg);

    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    tft.drawCentreString("PREVISAO DE 7 DIAS", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, border);
    drawSettingsButton();

    int startY = 46;
    for (size_t i = 0; i < daily.size() && i < 5; i++) {
        int y = startY + i * 48;

        tft.setTextColor(accent, bg);
        tft.setTextFont(2);
        tft.drawString(daily[i].dayName, 20, y + 8);

        tft.setTextColor(textM, bg);
        tft.setTextFont(1);
        tft.drawString(sanitizeText(daily[i].weatherDesc), 85, y + 12);

        char tempRange[20];
        snprintf(tempRange, sizeof(tempRange), "%.0f* / %.0f*", daily[i].tempMin, daily[i].tempMax);
        tft.setTextColor(textW, bg);
        tft.setTextFont(2);
        tft.drawRightString(tempRange, 220, y + 8, 2);

        tft.drawFastHLine(12, y + 40, 216, border);
    }

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 2) tft.fillCircle(dotX + i * 10, 304, 2, accent);
        else tft.drawCircle(dotX + i * 10, 304, 1, border);
    }
}

void DisplayManager::drawPageAir(const AirQuality& air, const CurrentWeather& current) {
    uint16_t bg = isPixel() ? PIXEL_BG : SWISS_BG;
    uint16_t textW = isPixel() ? PIXEL_TEXT : SWISS_TEXT_WHITE;
    uint16_t textM = isPixel() ? PIXEL_MUTED : SWISS_TEXT_MUTED;
    uint16_t accent = isPixel() ? PIXEL_AMBER : SWISS_ORANGE;
    uint16_t green = isPixel() ? PIXEL_GREEN : SWISS_GREEN;
    uint16_t border = isPixel() ? PIXEL_BORDER : SWISS_BORDER;

    tft.fillScreen(bg);

    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    tft.drawCentreString("QUALIDADE DO AR", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, border);
    drawSettingsButton();

    char aqiBuf[16];
    snprintf(aqiBuf, sizeof(aqiBuf), "AQI %d", air.aqi);
    tft.setTextColor(air.levelColor, bg);
    tft.setTextFont(4);
    tft.drawCentreString(aqiBuf, 120, 52, 4);

    String desc = sanitizeText(air.levelDesc);
    desc.toUpperCase();
    tft.setTextColor(textM, bg);
    tft.setTextFont(2);
    tft.drawCentreString(desc, 120, 84, 2);

    tft.drawFastHLine(12, 114, 216, border);
    tft.drawFastHLine(12, 194, 216, border);
    tft.drawFastHLine(12, 274, 216, border);
    tft.drawFastVLine(120, 114, 160, border);

    char pm25[16];
    snprintf(pm25, sizeof(pm25), "%.1f ug/m3", air.pm25);
    drawStat(20, 126, 150, "PM 2.5", pm25, textW, textM, bg);

    char pm10[16];
    snprintf(pm10, sizeof(pm10), "%.1f ug/m3", air.pm10);
    drawStat(132, 126, 150, "PM 10", pm10, textW, textM, bg);

    char oz[16];
    snprintf(oz, sizeof(oz), "%.0f ug/m3", air.ozone);
    drawStat(20, 206, 230, "OZONIO", oz, textW, textM, bg);

    char uv[16];
    snprintf(uv, sizeof(uv), "Nivel %d", current.uvIndex);
    drawStat(132, 206, 230, "INDICE UV", uv, (current.uvIndex >= 6) ? accent : green, textM, bg);

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 3) tft.fillCircle(dotX + i * 10, 304, 2, accent);
        else tft.drawCircle(dotX + i * 10, 304, 1, border);
    }
}

void DisplayManager::drawPageSettings(const AppSettings& settings, const String& ip, bool dataStale) {
    uint16_t bg = isPixel() ? PIXEL_BG : SWISS_BG;
    uint16_t cardBg = isPixel() ? PIXEL_CARD_BG : SWISS_BG;
    uint16_t textW = isPixel() ? PIXEL_TEXT : SWISS_TEXT_WHITE;
    uint16_t textM = isPixel() ? PIXEL_MUTED : SWISS_TEXT_MUTED;
    uint16_t accent = isPixel() ? PIXEL_AMBER : SWISS_ORANGE;
    uint16_t green = isPixel() ? PIXEL_GREEN : SWISS_GREEN;
    uint16_t border = isPixel() ? PIXEL_BORDER : SWISS_BORDER;

    tft.fillScreen(bg);

    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    tft.drawCentreString("STATUS DA ESTACAO", 120, 14, 2);
    tft.drawFastHLine(12, 38, 216, border);

    tft.setTextColor(textM, bg);
    tft.setTextFont(1);
    tft.drawString("ENDERECO IP", 20, 52);
    tft.setTextColor(accent, bg);
    tft.setTextFont(2);
    tft.drawString(ip, 20, 72);

    if (dataStale) {
        tft.setTextColor(COLOR_RED, bg);
        tft.setTextFont(1);
        tft.drawRightString("DADO DESATUALIZADO", 220, 72, 1);
    }

    tft.drawFastHLine(12, 100, 216, border);

    tft.setTextColor(textM, bg);
    tft.setTextFont(1);
    tft.drawString("CIDADE CONFIGURADA", 20, 114);
    tft.setTextColor(textW, bg);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(settings.cityName), 20, 134);

    tft.drawFastHLine(12, 162, 216, border);

    // BOTÃO "PROXIMO ESTILO" — toque troca de tema (zona: THEME_TAP_X1..Y2)
    tft.setTextColor(textM, bg);
    tft.setTextFont(1);
    tft.drawString("ESTILO VISUAL ATIVO (toque p/ trocar)", 20, 176);
    tft.fillRoundRect(THEME_TAP_X1, THEME_TAP_Y1 + 12, THEME_TAP_X2 - THEME_TAP_X1, 20, 4, cardBg);
    tft.drawRoundRect(THEME_TAP_X1, THEME_TAP_Y1 + 12, THEME_TAP_X2 - THEME_TAP_X1, 20, 4, border);
    tft.setTextColor(accent, cardBg);
    tft.setTextFont(2);
    String themeName = (settings.theme == THEME_PIXEL) ? "Pixel Art (Retro)" : "Swiss Minimalist (Dieter Rams)";
    tft.drawString(themeName, 20, THEME_TAP_Y1 + 16);

    tft.drawFastHLine(12, 224, 216, border);

    tft.setTextColor(textM, bg);
    tft.setTextFont(1);
    tft.drawString("FIRMWARE", 20, 238);
    tft.setTextColor(green, bg);
    tft.setTextFont(2);
    tft.drawString("Atmos BR v1.6.0 (Open-Source)", 20, 258);

    int dotX = 100;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == 4) tft.fillCircle(dotX + i * 10, 304, 2, accent);
        else tft.drawCircle(dotX + i * 10, 304, 1, border);
    }
}
