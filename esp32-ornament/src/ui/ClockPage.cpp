#include "ClockPage.h"

ClockPage::ClockPage() : _lastSecond(-1), _lastMinute(-1), _forceRedraw(true) {}
ClockPage::~ClockPage() {}

void ClockPage::render(DisplayManager& display, const TimeData& time) {
    int w = display.width();
    int contentY = STATUS_BAR_H;

    // Only redraw when needed (second changes or minute changes)
    bool secondChanged = (time.second != _lastSecond);
    bool minuteChanged = (time.minute != _lastMinute);

    if (_forceRedraw || minuteChanged) {
        // Full redraw every minute
        display.getTFT().fillRect(0, contentY, w, display.height() - contentY, TFT_BLACK);

        // Render digital time
        renderDigital(display, time);

        _forceRedraw = false;
        _lastMinute = time.minute;
    } else if (secondChanged) {
        // Only update the seconds portion
        int rectW = 60;
        int rectH = 30;
        int secX = w / 2 + 30;
        int secY = contentY + 40;
        display.getTFT().fillRect(secX - 8, secY - 12, rectW, rectH, TFT_BLACK);

        // Redraw seconds in red
        char secStr[8];
        snprintf(secStr, sizeof(secStr), ":%02d", time.second);
        display.getTFT().setTextColor(TFT_GREEN, TFT_BLACK);
        display.getTFT().setTextDatum(ML_DATUM);
        display.getTFT().drawString(secStr, secX, secY, 7);
    }

    _lastSecond = time.second;
}

void ClockPage::renderDigital(DisplayManager& display, const TimeData& time) {
    int w = display.width();
    int contentY = STATUS_BAR_H + 10;

    // Large time display (HH:MM — font 7)
    char timeStr[16];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", time.hour, time.minute);

    display.getTFT().setTextColor(TFT_WHITE, TFT_BLACK);
    display.getTFT().setTextDatum(TC_DATUM);
    display.getTFT().drawString(timeStr, w / 2 - 15, contentY + 30, 7);

    // Seconds in smaller font, in green
    char secStr[8];
    snprintf(secStr, sizeof(secStr), ":%02d", time.second);
    display.getTFT().setTextColor(TFT_GREEN, TFT_BLACK);
    display.getTFT().setTextDatum(ML_DATUM);
    display.getTFT().drawString(secStr, w / 2 + 30, contentY + 30, 7);

    // Date in Chinese (or English fallback)
    display.getTFT().setTextColor(TFT_CYAN, TFT_BLACK);
    display.getTFT().setTextDatum(TC_DATUM);
    display.getTFT().drawString(time.dateStr, w / 2, contentY + 100, 4);

    // Weekday
    display.getTFT().setTextColor(TFT_YELLOW, TFT_BLACK);
    display.getTFT().setTextDatum(TC_DATUM);
    display.getTFT().drawString(time.weekdayStr, w / 2, contentY + 130, 2);
}

void ClockPage::renderAnalog(DisplayManager& display, const TimeData& time) {
    int w = display.width();
    int h = display.height();
    int contentY = STATUS_BAR_H;
    int contentH = h - contentY;

    int cx = w / 2;
    int cy = contentY + contentH / 2;
    int r = min(w, contentH) / 2 - 15;

    _drawClockFace(display, cx, cy, r);
    _drawHands(display, cx, cy, r, time.hour, time.minute, time.second);
}

void ClockPage::_drawClockFace(DisplayManager& display, int cx, int cy, int r) {
    auto& tft = display.getTFT();

    // Face circle
    tft.fillCircle(cx, cy, r, TFT_BLACK);
    tft.drawCircle(cx, cy, r, TFT_WHITE);

    // Hour markers
    for (int i = 0; i < 12; i++) {
        float angle = i * 30 * DEG_TO_RAD - 90 * DEG_TO_RAD;
        int x1 = cx + cos(angle) * (r - 4);
        int y1 = cy + sin(angle) * (r - 4);
        int x2 = cx + cos(angle) * (r - (i % 3 == 0 ? 12 : 8));
        int y2 = cy + sin(angle) * (r - (i % 3 == 0 ? 12 : 8));
        tft.drawLine(x1, y1, x2, y2, (i % 3 == 0) ? TFT_RED : TFT_WHITE);
    }

    // Center dot
    tft.fillCircle(cx, cy, 4, TFT_RED);
}

void ClockPage::_drawHands(DisplayManager& display, int cx, int cy, int r, int hour, int min, int sec) {
    auto& tft = display.getTFT();

    // Hour hand
    float hAngle = ((hour % 12) * 30 + min * 0.5) * DEG_TO_RAD - 90 * DEG_TO_RAD;
    tft.drawLine(cx, cy, cx + cos(hAngle) * (r * 0.5), cy + sin(hAngle) * (r * 0.5), TFT_WHITE);

    // Minute hand
    float mAngle = (min * 6 + sec * 0.1) * DEG_TO_RAD - 90 * DEG_TO_RAD;
    tft.drawLine(cx, cy, cx + cos(mAngle) * (r * 0.7), cy + sin(mAngle) * (r * 0.7), TFT_CYAN);

    // Second hand (red, thin)
    float sAngle = sec * 6 * DEG_TO_RAD - 90 * DEG_TO_RAD;
    tft.drawLine(cx, cy, cx + cos(sAngle) * (r * 0.85), cy + sin(sAngle) * (r * 0.85), TFT_RED);
}
