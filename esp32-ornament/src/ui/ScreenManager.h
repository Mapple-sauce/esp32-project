#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"
#include "../time/TimeManager.h"
#include "../sensors/DHT22Sensor.h"
#include "../network/BilibiliClient.h"
#include "../network/WeatherClient.h"
#include "ClockPage.h"
#include "WeatherPage.h"
#include "BilibiliPage.h"
#include "IndoorPage.h"

class ScreenManager {
public:
    ScreenManager();
    ~ScreenManager();

    // Initialize pages
    void begin(DisplayManager& display);

    // Main render call — call every loop()
    void update(
        DisplayManager& display,
        TimeManager& timeMgr,
        const IndoorData& indoor,
        const WeatherData& weather,
        const BilibiliData& bilibili,
        bool wifiConnected
    );

    // Force refresh current page
    void refresh() { _forceRedraw = true; }

    // Manually switch to next/previous page
    void nextPage();
    void prevPage();

    // Set page by index
    void setPage(int index);

    // Get current page index
    int getCurrentPage() const { return _currentPage; }

    // Get total page count
    int getPageCount() const { return _pageCount; }

    // Check if page should auto-cycle
    void setAutoCycle(bool enabled) { _autoCycle = enabled; }

private:
    // Pages
    ClockPage _clockPage;
    WeatherPage _weatherPage;
    BilibiliPage _bilibiliPage;
    IndoorPage _indoorPage;

    // Page state
    int _currentPage;
    int _pageCount;
    bool _autoCycle;
    unsigned long _lastSwitchMs;
    bool _forceRedraw;

    // Last known data (for efficient redraw)
    TimeData _lastTime;
    bool _hasRenderedOnce;

    // Render a single page
    void _renderPage(
        DisplayManager& display,
        TimeManager& timeMgr,
        const IndoorData& indoor,
        const WeatherData& weather,
        const BilibiliData& bilibili,
        bool wifiConnected
    );
};

#endif // SCREEN_MANAGER_H
