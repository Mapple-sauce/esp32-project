#ifndef CLOCK_PAGE_H
#define CLOCK_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"

class ClockPage {
public:
    ClockPage();
    ~ClockPage();

    void render(DisplayManager& display, const TimeData& time);
    void renderAnalog(DisplayManager& display, const TimeData& time);
    void renderDigital(DisplayManager& display, const TimeData& time);

private:
    int _lastSecond;  // For efficient redraw (only update seconds)
    int _lastMinute;  // For minute-hand redraw threshold
    bool _forceRedraw;

    // Draw analog clock face
    void _drawClockFace(DisplayManager& display, int cx, int cy, int r);
    // Draw clock hands
    void _drawHands(DisplayManager& display, int cx, int cy, int r, int hour, int min, int sec);
};

#endif // CLOCK_PAGE_H
