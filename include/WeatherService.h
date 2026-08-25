#pragma once

#include <Arduino.h>
#include <vector>
#include "Config.h"

class WeatherService {
public:
    WeatherService();
    
    bool updateWeatherData(float lat, float lon, CurrentWeather& current, 
                           std::vector<HourlyForecast>& hourly, 
                           std::vector<DailyForecast>& daily);
                           
    bool updateAirQuality(float lat, float lon, AirQuality& air);

    static String getWeatherDescription(int code);
    static String getDayNamePT(int dayOfWeek);
    static String getAdvice(const CurrentWeather& current, const AirQuality& air);
};
