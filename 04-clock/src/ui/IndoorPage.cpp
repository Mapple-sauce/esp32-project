#include "IndoorPage.h"

IndoorPage::IndoorPage() : _forceRedraw(true) {}
IndoorPage::~IndoorPage() {}

void IndoorPage::render(DisplayManager& display, const IndoorData& data) {
    auto& tft = display.getTFT();
    int w = display.width();
    int contentY = STATUS_BAR_H;

    if (_forceRedraw) {
        tft.fillRect(0, contentY, w, display.height() - contentY, TFT_BLACK);
        _forceRedraw = false;
    }

    if (!data.valid) {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Sensor warming up...", w / 2, display.height() / 2, 1);
        return;
    }

    int centerX = w / 2;
    int mid = contentY + (display.height() - contentY) / 2;

    // Temperature (left half or top)
    char tempStr[16];
    snprintf(tempStr, sizeof(tempStr), "%.1f", data.temperature);

    uint16_t tempColor = DisplayManager::heatColor(data.temperature, 15, 35);

    tft.fillRoundRect(centerX - 100, mid - 70, 80, 70, 8, TFT_NAVY);
    tft.setTextColor(TFT_CYAN, TFT_NAVY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Indoor Temp", centerX - 60, mid - 60, 1);

    tft.setTextColor(tempColor, TFT_NAVY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(tempStr, centerX - 60, mid - 25, 7);

    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("°C", centerX - 60, mid + 15, 2);

    // Humidity (right half)
    char humStr[16];
    snprintf(humStr, sizeof(humStr), "%.0f", data.humidity);

    tft.fillRoundRect(centerX + 20, mid - 70, 80, 70, 8, TFT_NAVY);
    tft.setTextColor(TFT_CYAN, TFT_NAVY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Humidity", centerX + 60, mid - 60, 1);

    tft.setTextColor(TFT_SKYBLUE, TFT_NAVY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(humStr, centerX + 60, mid - 25, 7);

    tft.setTextColor(TFT_WHITE, TFT_NAVY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("%", centerX + 60, mid + 15, 2);

    // Sensor status at bottom
    unsigned long age = (data.lastUpdate > 0) ? (millis() - data.lastUpdate) / 1000 : 0;
    char statusStr[32];
    snprintf(statusStr, sizeof(statusStr), "Updated %lu s ago", age);
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(statusStr, centerX, display.height() - 15, 1);
}
