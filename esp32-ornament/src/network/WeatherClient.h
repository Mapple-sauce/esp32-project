#ifndef WEATHER_CLIENT_H
#define WEATHER_CLIENT_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

class WeatherClient {
public:
    WeatherClient();
    ~WeatherClient();

    // Set location (city ID or "lon,lat")
    void setLocation(const char* cityId);

    // Fetch current weather from QWeather API
    bool fetch();

    // Get latest data
    WeatherData getData() const { return _data; }

    // Check if data is fresh
    bool isDataFresh() const;

    // Get weather icon character (emoji-like mapping)
    static const char* getWeatherIcon(const char* iconCode);

    // Get weather condition in Chinese
    static const char* getConditionText(const char* iconCode);

private:
    WeatherData _data;
    char _location[32];
    unsigned long _lastFetchMs;

    // Parse QWeather JSON response
    bool _parseResponse(const String& json);
};

#endif // WEATHER_CLIENT_H
