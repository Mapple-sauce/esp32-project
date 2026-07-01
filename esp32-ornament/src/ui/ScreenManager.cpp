#include "ScreenManager.h"

ScreenManager::ScreenManager()
    : _currentPage(0)
    , _pageCount(4)  // Clock, Weather, Bilibili, Indoor
    , _autoCycle(true)
    , _lastSwitchMs(0)
    , _forceRedraw(true)
    , _hasRenderedOnce(false)
{}

ScreenManager::~ScreenManager() {}

void ScreenManager::begin(DisplayManager& display) {
    memset(&_lastTime, 0, sizeof(_lastTime));
    _forceRedraw = true;
}

void ScreenManager::update(
    DisplayManager& display,
    TimeManager& timeMgr,
    const IndoorData& indoor,
    const WeatherData& weather,
    const BilibiliData& bilibili,
    bool wifiConnected)
{
    // Auto-cycle page
    if (_autoCycle && (millis() - _lastSwitchMs > SCREEN_CYCLE_MS)) {
        _currentPage = (_currentPage + 1) % _pageCount;
        _forceRedraw = true;
        _lastSwitchMs = millis();
    }

    _renderPage(display, timeMgr, indoor, weather, bilibili, wifiConnected);
    _hasRenderedOnce = true;
}

void ScreenManager::_renderPage(
    DisplayManager& display,
    TimeManager& timeMgr,
    const IndoorData& indoor,
    const WeatherData& weather,
    const BilibiliData& bilibili,
    bool wifiConnected)
{
    // Get current time
    TimeData time = timeMgr.getTimeData();

    // Draw status bar (always)
    display.drawStatusBar(_currentPage, _pageCount,
                          time.timeStr, wifiConnected,
                          _forceRedraw || !_hasRenderedOnce);

    // Render current page
    switch (_currentPage) {
        case 0:  // Clock
            _clockPage.render(display, time);
            break;

        case 1:  // Weather
            _weatherPage.render(display, weather, indoor);
            break;

        case 2:  // Bilibili
            _bilibiliPage.render(display, bilibili);
            break;

        case 3:  // Indoor
            _indoorPage.render(display, indoor);
            break;
    }

    _forceRedraw = false;
}

void ScreenManager::nextPage() {
    _currentPage = (_currentPage + 1) % _pageCount;
    _forceRedraw = true;
    _lastSwitchMs = millis();
}

void ScreenManager::prevPage() {
    _currentPage = (_currentPage - 1 + _pageCount) % _pageCount;
    _forceRedraw = true;
    _lastSwitchMs = millis();
}

void ScreenManager::setPage(int index) {
    if (index >= 0 && index < _pageCount) {
        _currentPage = index;
        _forceRedraw = true;
        _lastSwitchMs = millis();
    }
}
