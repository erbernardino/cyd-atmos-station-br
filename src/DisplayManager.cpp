#include "DisplayManager.h"
#include "WeatherService.h"

DisplayManager::DisplayManager() : currentBrightness(200) {}

void DisplayManager::init() {
    tft.init();
    tft.setRotation(0); // 240x320 vertical (padrão Atmos)
    tft.fillScreen(COLOR_BG);

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
                // Maiúsculas com acento
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: result += 'A'; break; // À Á Â Ã Ä Å
                case 0x87: result += 'C'; break; // Ç
                case 0x88: case 0x89: case 0x8A: case 0x8B: result += 'E'; break; // È É Ê Ë
                case 0x8C: case 0x8D: case 0x8E: case 0x8F: result += 'I'; break; // Ì Í Î Ï
                case 0x91: result += 'N'; break; // Ñ
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: result += 'O'; break; // Ò Ó Ô Õ Ö
                case 0x99: case 0x9A: case 0x9B: case 0x9C: result += 'U'; break; // Ù Ú Û Ü
                // Minúsculas com acento
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: result += 'a'; break; // à á â ã ä å
                case 0xA7: result += 'c'; break; // ç
                case 0xA8: case 0xA9: case 0xAA: case 0xAB: result += 'e'; break; // è é ê ë
                case 0xAC: case 0xAD: case 0xAE: case 0xAF: result += 'i'; break; // ì í î ï
                case 0xB1: result += 'n'; break; // ñ
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: result += 'o'; break; // ò ó ô õ ö
                case 0xB9: case 0xBA: case 0xBB: case 0xBC: result += 'u'; break; // ù ú û ü
                default: result += ' '; break;
            }
        } else if (c < 128) {
            result += (char)c;
        }
    }
    return result;
}

void DisplayManager::drawCard(int x, int y, int w, int h, const String& title) {
    tft.fillRoundRect(x, y, w, h, 8, COLOR_CARD_BG);
    tft.drawRoundRect(x, y, w, h, 8, COLOR_CARD_BORDER);
    if (title.length() > 0) {
        tft.setTextColor(COLOR_TEXT_MUTED, COLOR_CARD_BG);
        tft.setTextFont(1);
        tft.drawString(sanitizeText(title), x + 8, y + 6);
    }
}

void DisplayManager::drawWeatherIcon(int x, int y, int weatherCode, bool isDay, int size) {
    if (weatherCode == 0 || weatherCode == 1) {
        if (isDay) {
            tft.fillCircle(x, y, 10 * size, COLOR_YELLOW);
            tft.drawCircle(x, y, 14 * size, COLOR_ORANGE);
        } else {
            tft.fillCircle(x, y, 10 * size, COLOR_TEXT_WHITE);
            tft.fillCircle(x + 4 * size, y - 2 * size, 8 * size, COLOR_BG);
        }
    } else if (weatherCode >= 2 && weatherCode <= 3) {
        tft.fillCircle(x - 5 * size, y, 7 * size, COLOR_TEXT_MUTED);
        tft.fillCircle(x + 5 * size, y, 9 * size, COLOR_TEXT_MUTED);
        tft.fillRoundRect(x - 10 * size, y + 2 * size, 22 * size, 8 * size, 4, COLOR_TEXT_MUTED);
    } else if (weatherCode >= 51 && weatherCode <= 82) {
        tft.fillCircle(x, y - 3 * size, 8 * size, COLOR_TEXT_MUTED);
        tft.drawLine(x - 4 * size, y + 6 * size, x - 6 * size, y + 12 * size, COLOR_BLUE);
        tft.drawLine(x, y + 6 * size, x - 2 * size, y + 12 * size, COLOR_BLUE);
        tft.drawLine(x + 4 * size, y + 6 * size, x + 2 * size, y + 12 * size, COLOR_BLUE);
    } else if (weatherCode >= 95) {
        tft.fillCircle(x, y - 4 * size, 8 * size, COLOR_CARD_BORDER);
        tft.drawLine(x, y + 4 * size, x - 3 * size, y + 9 * size, COLOR_YELLOW);
        tft.drawLine(x - 3 * size, y + 9 * size, x + 2 * size, y + 9 * size, COLOR_YELLOW);
        tft.drawLine(x + 2 * size, y + 9 * size, x - 2 * size, y + 14 * size, COLOR_YELLOW);
    } else {
        tft.fillCircle(x, y, 8 * size, COLOR_CYAN);
    }
}

void DisplayManager::drawHeader(const String& city, const String& timeStr, int wifiRssi, ScreenPage page) {
    tft.fillRect(0, 0, 240, 26, COLOR_BG);

    // Cidade (Higienizada contra caracteres especiais que quebram fontes)
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_BG);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(city), 8, 4);

    // Hora
    tft.setTextColor(COLOR_CYAN, COLOR_BG);
    tft.drawRightString(timeStr, 232, 4, 2);

    // Indicador de Páginas
    int startX = 90;
    for (int i = 0; i < PAGE_COUNT; i++) {
        if (i == (int)page) {
            tft.fillCircle(startX + i * 12, 14, 3, COLOR_CYAN);
        } else {
            tft.drawCircle(startX + i * 12, 14, 2, COLOR_CARD_BORDER);
        }
    }
}

void DisplayManager::drawLoadingScreen(const String& status) {
    tft.fillScreen(COLOR_BG);
    tft.setTextColor(COLOR_CYAN, COLOR_BG);
    tft.setTextFont(4);
    tft.drawCentreString("ATMOS BR", 120, 100, 4);

    tft.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    tft.setTextFont(2);
    tft.drawCentreString("Estacao Meteorologica", 120, 140, 2);
    tft.drawCentreString(sanitizeText(status), 120, 180, 2);
}

void DisplayManager::drawPageNow(const CurrentWeather& weather, const AirQuality& air) {
    tft.fillRect(0, 26, 240, 294, COLOR_BG);

    // 1. Card Principal
    drawCard(8, 30, 224, 90, "");
    drawWeatherIcon(42, 75, weather.weatherCode, weather.isDay, 2);

    char tempStr[10];
    snprintf(tempStr, sizeof(tempStr), "%.0f*", weather.temperature);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(7);
    tft.drawString(tempStr, 80, 40);

    tft.setTextColor(COLOR_CYAN, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(weather.weatherDesc), 80, 92);

    char subStr[40];
    snprintf(subStr, sizeof(subStr), "Min %.0f*  Max %.0f*  Sens %.0f*", 
             weather.tempMin, weather.tempMax, weather.apparentTemperature);
    tft.setTextColor(COLOR_TEXT_MUTED, COLOR_CARD_BG);
    tft.setTextFont(1);
    tft.drawString(subStr, 80, 78);

    // 2. Grid de 4 Cards
    drawCard(8, 126, 108, 54, "VENTO");
    char windStr[16];
    snprintf(windStr, sizeof(windStr), "%.0f km/h", weather.windSpeed);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(windStr, 16, 148);

    drawCard(124, 126, 108, 54, "UMIDADE");
    char humStr[10];
    snprintf(humStr, sizeof(humStr), "%d %%", weather.humidity);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(humStr, 132, 148);

    drawCard(8, 186, 108, 54, "PRESSAO");
    char pressStr[16];
    snprintf(pressStr, sizeof(pressStr), "%.0f hPa", weather.pressure);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(pressStr, 16, 208);

    drawCard(124, 186, 108, 54, "QUALID. AR");
    tft.setTextColor(air.levelColor, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(air.levelDesc), 132, 208);

    // 3. Card Dica do Dia
    drawCard(8, 246, 224, 52, "DICA DO DIA");
    String advice = WeatherService::getAdvice(weather, air);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(1);
    tft.drawString(sanitizeText(advice), 16, 268);

    tft.setTextColor(COLOR_TEXT_MUTED, COLOR_BG);
    tft.drawCentreString("< Toque para navegar >", 120, 306, 1);
}

void DisplayManager::drawPageHourly(const std::vector<HourlyForecast>& hourly) {
    tft.fillRect(0, 26, 240, 294, COLOR_BG);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_BG);
    tft.setTextFont(2);
    tft.drawString("PREVISAO POR HORA", 8, 30);

    int cardY = 54;
    for (size_t i = 0; i < hourly.size() && i < 4; i++) {
        drawCard(8, cardY + i * 58, 224, 52, "");

        char hStr[10];
        snprintf(hStr, sizeof(hStr), "+%dh", hourly[i].hour + 1);
        tft.setTextColor(COLOR_CYAN, COLOR_CARD_BG);
        tft.setTextFont(2);
        tft.drawString(hStr, 16, cardY + i * 58 + 16);

        drawWeatherIcon(70, cardY + i * 58 + 26, hourly[i].weatherCode, true, 1);

        char tStr[10];
        snprintf(tStr, sizeof(tStr), "%.0f *C", hourly[i].temperature);
        tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
        tft.setTextFont(2);
        tft.drawString(tStr, 100, cardY + i * 58 + 16);

        char rStr[16];
        snprintf(rStr, sizeof(rStr), "Chuva: %d%%", hourly[i].rainProbability);
        tft.setTextColor(COLOR_BLUE, COLOR_CARD_BG);
        tft.setTextFont(1);
        tft.drawString(rStr, 160, cardY + i * 58 + 20);
    }
}

void DisplayManager::drawPageWeek(const std::vector<DailyForecast>& daily) {
    tft.fillRect(0, 26, 240, 294, COLOR_BG);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_BG);
    tft.setTextFont(2);
    tft.drawString("PREVISAO DE 7 DIAS", 8, 30);

    int startY = 54;
    for (size_t i = 0; i < daily.size() && i < 5; i++) {
        drawCard(8, startY + i * 48, 224, 44, "");

        tft.setTextColor(COLOR_CYAN, COLOR_CARD_BG);
        tft.setTextFont(2);
        tft.drawString(sanitizeText(daily[i].dayName), 16, startY + i * 48 + 12);

        drawWeatherIcon(80, startY + i * 48 + 22, daily[i].weatherCode, true, 1);

        tft.setTextColor(COLOR_TEXT_MUTED, COLOR_CARD_BG);
        tft.setTextFont(1);
        tft.drawString(sanitizeText(daily[i].weatherDesc), 105, startY + i * 48 + 8);

        char tempRange[20];
        snprintf(tempRange, sizeof(tempRange), "%.0f* / %.0f*", daily[i].tempMin, daily[i].tempMax);
        tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
        tft.setTextFont(2);
        tft.drawRightString(tempRange, 224, startY + i * 48 + 12, 2);
    }
}

void DisplayManager::drawPageAir(const AirQuality& air, const CurrentWeather& current) {
    tft.fillRect(0, 26, 240, 294, COLOR_BG);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_BG);
    tft.setTextFont(2);
    tft.drawString("QUALIDADE DO AR & AMBIENTE", 8, 30);

    drawCard(8, 54, 224, 70, "INDICE EUROPEU (AQI)");
    char aqiStr[16];
    snprintf(aqiStr, sizeof(aqiStr), "AQI %d", air.aqi);
    tft.setTextColor(air.levelColor, COLOR_CARD_BG);
    tft.setTextFont(4);
    tft.drawString(aqiStr, 16, 76);
    tft.drawString(sanitizeText(air.levelDesc), 110, 76);

    drawCard(8, 130, 108, 54, "PM 2.5");
    char pm25Str[16];
    snprintf(pm25Str, sizeof(pm25Str), "%.1f ug/m3", air.pm25);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(pm25Str, 16, 152);

    drawCard(124, 130, 108, 54, "PM 10");
    char pm10Str[16];
    snprintf(pm10Str, sizeof(pm10Str), "%.1f ug/m3", air.pm10);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(pm10Str, 132, 152);

    drawCard(8, 190, 108, 54, "OZONIO");
    char ozStr[16];
    snprintf(ozStr, sizeof(ozStr), "%.0f ug/m3", air.ozone);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(ozStr, 16, 212);

    drawCard(124, 190, 108, 54, "INDICE UV");
    char uvStr[16];
    snprintf(uvStr, sizeof(uvStr), "Nivel %d", current.uvIndex);
    tft.setTextColor((current.uvIndex >= 6) ? COLOR_ORANGE : COLOR_GREEN, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(uvStr, 132, 212);
}

void DisplayManager::drawPageSettings(const AppSettings& settings, const String& ip) {
    tft.fillRect(0, 26, 240, 294, COLOR_BG);
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_BG);
    tft.setTextFont(2);
    tft.drawString("INFORMACOES DA ESTACAO", 8, 30);

    drawCard(8, 54, 224, 60, "ENDERECO IP NA REDE");
    tft.setTextColor(COLOR_CYAN, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(ip, 16, 78);

    drawCard(8, 120, 224, 60, "CIDADE CONFIGURADA");
    tft.setTextColor(COLOR_TEXT_WHITE, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(sanitizeText(settings.cityName), 16, 144);

    drawCard(8, 186, 224, 60, "BRILHO & MODO NOTURNO");
    char brStr[40];
    snprintf(brStr, sizeof(brStr), "Brilho: %d%% | Eco: %s", 
             (settings.brightness * 100) / 255, settings.ecoMode ? "ATIVO" : "DESATIVADO");
    tft.setTextColor(COLOR_TEXT_MUTED, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString(brStr, 16, 210);

    drawCard(8, 252, 224, 46, "VERSAO DO SOFTWARE");
    tft.setTextColor(COLOR_GREEN, COLOR_CARD_BG);
    tft.setTextFont(2);
    tft.drawString("Atmos BR v1.1.0 (Open-Source)", 16, 268);
}
