#include "WeatherPage.h"

WeatherPage::WeatherPage() : _forceRedraw(true) {}
WeatherPage::~WeatherPage() {}

void WeatherPage::render(DisplayManager& display, const WeatherData& weather, const IndoorData& indoor) {
    display.getTFT().fillRect(0, STATUS_BAR_H, display.width(), display.height() - STATUS_BAR_H, TFT_BLACK);
    _forceRedraw = false;

    if (!weather.valid) {
        display.getTFT().setTextColor(TFT_DARKGREY, TFT_BLACK);
        display.getTFT().setTextDatum(MC_DATUM);
        display.getTFT().drawString("Weather data unavailable", display.width() / 2, display.height() / 2, 1);
        return;
    }

    int w = display.width();
    int contentY = STATUS_BAR_H + 10;

    // Weather icon + condition on top row
    auto& tft = display.getTFT();

    // Draw weather icon (simple shapes)
    _drawWeatherIcon(display, weather.icon, w / 2 - 30, contentY, 40);

    // Condition text below icon
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(weather.condition, w / 2, contentY + 50, 2);

    // Large temperature
    _drawTemperature(display, weather.temp, contentY + 75);

    // Details
    _drawDetails(display, weather);

    // Indoor comparison (bottom)
    if (indoor.valid) {
        char indoorStr[32];
        snprintf(indoorStr, sizeof(indoorStr), "室内: %.1f°C / %.0f%%",
                 indoor.temperature, indoor.humidity);
        tft.setTextColor(TFT_CYAN, TFT_BLACK);
        tft.setTextDatum(BC_DATUM);
        tft.drawString(indoorStr, w / 2, display.height() - 10, 1);
    }
}

void WeatherPage::_drawWeatherIcon(DisplayManager& display, const char* iconCode, int x, int y, int size) {
    auto& tft = display.getTFT();
    int code = atoi(iconCode);

    // Simple icon drawing (replace with bitmap images if desired)
    int half = size / 2;

    switch (code) {
        case 100:  // Sunny — circle with rays
            tft.fillCircle(x, y, half / 2, TFT_YELLOW);
            for (int i = 0; i < 8; i++) {
                float a = i * 45 * DEG_TO_RAD;
                tft.drawLine(x + cos(a) * half * 0.35, y + sin(a) * half * 0.35,
                             x + cos(a) * half * 0.5, y + sin(a) * half * 0.5, TFT_YELLOW);
            }
            break;

        case 104:  // Overcast — grey circle
            tft.fillCircle(x, y, half / 2, TFT_LIGHTGREY);
            tft.drawCircle(x, y, half / 2, TFT_DARKGREY);
            break;

        case 101: case 102: case 103:  // Cloudy — sun + cloud
            tft.fillCircle(x - half / 4, y - half / 6, half / 3, TFT_YELLOW);
            tft.fillCircle(x + half / 6, y, half / 3, TFT_LIGHTGREY);
            break;

        case 305: case 306: case 307:  // Rain
            tft.fillCircle(x, y, half / 3, TFT_DARKGREY);
            // Rain drops
            for (int i = -1; i <= 1; i++) {
                tft.drawLine(x + i * half / 4, y + half / 3,
                             x + i * half / 4, y + half / 3 + 8, TFT_SKYBLUE);
            }
            break;

        default:  // Unknown
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextDatum(TC_DATUM);
            tft.drawString("?", x, y - half / 2, 2);
            break;
    }
}

void WeatherPage::_drawTemperature(DisplayManager& display, float temp, int y) {
    char tempStr[16];
    snprintf(tempStr, sizeof(tempStr), "%.0f°", temp);

    uint16_t color;
    if (temp >= 35) color = TFT_RED;
    else if (temp >= 28) color = TFT_ORANGE;
    else if (temp >= 20) color = TFT_YELLOW;
    else if (temp >= 10) color = TFT_CYAN;
    else color = TFT_BLUE;

    display.getTFT().setTextColor(color, TFT_BLACK);
    display.getTFT().setTextDatum(TC_DATUM);
    display.getTFT().drawString(tempStr, display.width() / 2, y, 7);
}

void WeatherPage::_drawDetails(DisplayManager& display, const WeatherData& weather) {
    int w = display.width();
    int y = STATUS_BAR_H + 145;

    auto& tft = display.getTFT();
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);

    tft.drawString("Humidity:", 20, y, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    char str[16];
    snprintf(str, sizeof(str), "%d%%", weather.humidity);
    tft.drawString(str, w / 2 - 10, y, 1);

    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.drawString("Wind:", w / 2 + 10, y, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    snprintf(str, sizeof(str), "%d level", weather.windScale);
    tft.drawString(str, w - 20, y, 1);

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Wind Speed:", 20, y + 20, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    snprintf(str, sizeof(str), "%.1f km/h", weather.windSpeed);
    tft.drawString(str, w - 20, y + 20, 1);
}
