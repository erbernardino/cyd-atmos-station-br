#include "WeatherService.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

WeatherService::WeatherService() {}

String WeatherService::getWeatherDescription(int code) {
    switch (code) {
        case 0: return "Ceu Limpo";
        case 1: return "Predom. Limpo";
        case 2: return "Parcial. Nublado";
        case 3: return "Nublado";
        case 45: return "Nevoeiro";
        case 48: return "Nevoeiro Raso";
        case 51: return "Garoa Leve";
        case 53: return "Garoa Moderada";
        case 55: return "Garoa Densa";
        case 61: return "Chuva Fraca";
        case 63: return "Chuva Moderada";
        case 65: return "Chuva Forte";
        case 71: return "Neve Fraca";
        case 73: return "Neve Moderada";
        case 75: return "Neve Forte";
        case 77: return "Granizo Fino";
        case 80: return "Pancadas Leves";
        case 81: return "Pancadas Chuva";
        case 82: return "Pancadas Fortes";
        case 85: return "Pancadas Neve";
        case 86: return "Nevasca";
        case 95: return "Tempestade";
        case 96: return "Temp. c/ Granizo";
        case 99: return "Temp. Severa";
        default: return "Variado";
    }
}

String WeatherService::getDayNamePT(int dayOfWeek) {
    switch (dayOfWeek % 7) {
        case 0: return "DOM";
        case 1: return "SEG";
        case 2: return "TER";
        case 3: return "QUA";
        case 4: return "QUI";
        case 5: return "SEX";
        case 6: return "SAB";
        default: return "---";
    }
}

String WeatherService::getAdvice(const CurrentWeather& current, const AirQuality& air) {
    if (current.uvIndex >= 8) return "UV Extremo: use protetor e oculos!";
    if (current.uvIndex >= 6) return "UV Alto: protecao solar indicada.";
    if (current.windGusts >= 50.0) return "Atencao: rajadas de vento fortes!";
    if (current.precipitation >= 5.0) return "Chuva forte: leve guarda-chuva!";
    if (current.temperature >= 32.0) return "Calor intenso: hidrate-se bem!";
    if (current.temperature <= 10.0) return "Frio: vista agasalho!";
    if (air.aqi >= 75) return "Ar Ruim: evite atividades ao ar livre.";
    return "Condicoes estaveis e agradaveis.";
}

bool WeatherService::updateWeatherData(float lat, float lon, CurrentWeather& current, 
                                       std::vector<HourlyForecast>& hourly, 
                                       std::vector<DailyForecast>& daily) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    char url[512];
    snprintf(url, sizeof(url),
        "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f&current=temperature_2m,relative_humidity_2m,apparent_temperature,dew_point_2m,precipitation,weather_code,pressure_msl,cloud_cover,visibility,wind_speed_10m,wind_direction_10m,wind_gusts_10m,is_day&hourly=temperature_2m,precipitation_probability,weather_code&daily=weather_code,temperature_2m_max,temperature_2m_min,precipitation_probability_max,uv_index_max&forecast_hours=12&forecast_days=7&timezone=auto",
        lat, lon);

    http.begin(client, url);
    http.setTimeout(10000);
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        Serial.printf("[Weather] Falha HTTP: %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.printf("[Weather] Erro JSON: %s\n", error.c_str());
        return false;
    }

    // 1. Clima Atual
    JsonObject cur = doc["current"];
    current.temperature = cur["temperature_2m"] | 0.0f;
    current.apparentTemperature = cur["apparent_temperature"] | current.temperature;
    current.humidity = cur["relative_humidity_2m"] | 0;
    current.pressure = cur["pressure_msl"] | 1013.0f;
    current.precipitation = cur["precipitation"] | 0.0f;
    current.windSpeed = cur["wind_speed_10m"] | 0.0f;
    current.windGusts = cur["wind_gusts_10m"] | current.windSpeed;
    current.windDirection = cur["wind_direction_10m"] | 0;
    current.dewPoint = cur["dew_point_2m"] | 0.0f;
    current.visibilityKm = (float)(cur["visibility"] | 10000) / 1000.0f;
    current.cloudCover = cur["cloud_cover"] | 0;
    current.weatherCode = cur["weather_code"] | 0;
    current.weatherDesc = getWeatherDescription(current.weatherCode);
    current.isDay = cur["is_day"] | 1;

    // 2. Previsão Diária (Hoje e 7 dias)
    JsonObject d = doc["daily"];
    JsonArray d_time = d["time"];
    JsonArray d_tmax = d["temperature_2m_max"];
    JsonArray d_tmin = d["temperature_2m_min"];
    JsonArray d_rain = d["precipitation_probability_max"];
    JsonArray d_code = d["weather_code"];
    JsonArray d_uv   = d["uv_index_max"];

    if (d_tmax.size() > 0) {
        current.tempMax = d_tmax[0] | current.temperature;
        current.tempMin = d_tmin[0] | current.temperature;
        current.uvIndex = d_uv[0] | 0;
    }

    daily.clear();
    for (size_t i = 0; i < d_time.size() && i < 7; i++) {
        DailyForecast df;
        df.dayName = (i == 0) ? "HOJE" : ("D+" + String(i));
        df.tempMax = d_tmax[i] | 0.0f;
        df.tempMin = d_tmin[i] | 0.0f;
        df.rainProbability = d_rain[i] | 0;
        df.weatherCode = d_code[i] | 0;
        df.weatherDesc = getWeatherDescription(df.weatherCode);
        daily.push_back(df);
    }

    // 3. Previsão Horária (Próximas 12h)
    JsonObject h = doc["hourly"];
    JsonArray h_temp = h["temperature_2m"];
    JsonArray h_rain = h["precipitation_probability"];
    JsonArray h_code = h["weather_code"];

    hourly.clear();
    for (size_t i = 0; i < h_temp.size() && i < 12; i++) {
        HourlyForecast hf;
        hf.hour = i;
        hf.temperature = h_temp[i] | 0.0f;
        hf.rainProbability = h_rain[i] | 0;
        hf.weatherCode = h_code[i] | 0;
        hourly.push_back(hf);
    }

    Serial.println("[Weather] Dados meteorologicos atualizados com sucesso!");
    return true;
}

bool WeatherService::updateAirQuality(float lat, float lon, AirQuality& air) {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient http;
    char url[512];
    snprintf(url, sizeof(url),
        "https://air-quality-api.open-meteo.com/v1/air-quality?latitude=%.4f&longitude=%.4f&current=european_aqi,pm2_5,pm10,ozone,dust&timezone=auto",
        lat, lon);

    http.begin(client, url);
    http.setTimeout(10000);
    int httpCode = http.GET();

    if (httpCode != HTTP_CODE_OK) {
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) return false;

    JsonObject cur = doc["current"];
    air.aqi = cur["european_aqi"] | 25;
    air.pm25 = cur["pm2_5"] | 5.0f;
    air.pm10 = cur["pm10"] | 10.0f;
    air.ozone = cur["ozone"] | 20.0f;
    air.dust = cur["dust"] | 0.0f;

    if (air.aqi <= 20) {
        air.levelDesc = "Excelente";
        air.levelColor = COLOR_GREEN;
    } else if (air.aqi <= 40) {
        air.levelDesc = "Bom";
        air.levelColor = COLOR_GREEN;
    } else if (air.aqi <= 60) {
        air.levelDesc = "Moderado";
        air.levelColor = COLOR_YELLOW;
    } else if (air.aqi <= 80) {
        air.levelDesc = "Ruim";
        air.levelColor = COLOR_ORANGE;
    } else {
        air.levelDesc = "Muito Ruim";
        air.levelColor = COLOR_RED;
    }

    Serial.println("[Air] Qualidade do ar atualizada!");
    return true;
}
