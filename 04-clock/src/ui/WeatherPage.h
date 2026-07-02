#ifndef WEATHER_PAGE_H
#define WEATHER_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"
#include "../network/WeatherClient.h"

class WeatherPage {
public:
    WeatherPage();
    ~WeatherPage();

    void render(DisplayManager& display, const WeatherData& weather, const IndoorData& indoor);

private:
    bool _forceRedraw;

    // Draw animated weather icon area
    void _drawWeatherIcon(DisplayManager& display, const char* iconCode, int x, int y, int size);
    // Draw outdoor temperature large
    void _drawTemperature(DisplayManager& display, float temp, int y);
    // Draw weather details (humidity, wind, etc.)
    void _drawDetails(DisplayManager& display, const WeatherData& weather);
};

#endif // WEATHER_PAGE_H
