#include "TimeManager.h"

TimeManager::TimeManager()
    : _ntp(_udp, NTP_SERVER1, TIMEZONE_OFFSET_SECS, 60000)
    , _timeValid(false)
    , _lastSyncMs(0)
{
    memset(&_data, 0, sizeof(_data));
}

TimeManager::~TimeManager() {}

void TimeManager::begin() {
    _ntp.begin();
    _ntp.setTimeOffset(TIMEZONE_OFFSET_SECS);
}

bool TimeManager::update() {
    if (!_ntp.update()) {
        // Force update (blocks briefly, NTPClient fallback)
        if (!_ntp.forceUpdate()) {
            Serial.println("[Time] NTP sync failed");
            return false;
        }
    }

    _timeValid = true;
    _lastSyncMs = millis();
    _updateStrings();
    Serial.printf("[Time] Synced: %s %s\n", _data.dateStr, _data.timeStr);
    return true;
}

TimeData TimeManager::getTimeData() {
    if (_timeValid) {
        unsigned long epoch = _ntp.getEpochTime();
        time_t raw = epoch;
        struct tm* ti = localtime(&raw);

        _data.year = ti->tm_year + 1900;
        _data.month = ti->tm_mon + 1;
        _data.day = ti->tm_mday;
        _data.hour = ti->tm_hour;
        _data.minute = ti->tm_min;
        _data.second = ti->tm_sec;
        _data.weekday = ti->tm_wday == 0 ? 7 : ti->tm_wday;  // Convert Sunday=0 → Sunday=7
        _data.valid = true;

        _updateStrings();
    }
    return _data;
}

const char* TimeManager::getTimeString() {
    if (_timeValid) {
        getTimeData();  // Refresh
    }
    return _data.timeStr;
}

const char* TimeManager::getDateString() {
    if (_timeValid) {
        getTimeData();
    }
    return _data.dateStr;
}

const char* TimeManager::getWeekdayString() {
    if (_timeValid) {
        getTimeData();
    }
    return _data.weekdayStr;
}

void TimeManager::setManual(int year, int month, int day, int hour, int min, int sec) {
    _data.year = year;
    _data.month = month;
    _data.day = day;
    _data.hour = hour;
    _data.minute = min;
    _data.second = sec;
    _data.valid = true;
    _timeValid = true;
    _updateStrings();
}

unsigned long TimeManager::getUptime() const {
    if (_timeValid) return millis() - _lastSyncMs;
    return 0;
}

void TimeManager::_updateStrings() {
    // Get raw time_t from NTP for accurate formatting
    if (!_timeValid) return;

    unsigned long epoch = _ntp.getEpochTime();
    time_t raw = epoch;
    struct tm* ti = localtime(&raw);

    // Format time "14:25:30"
    snprintf(_data.timeStr, sizeof(_data.timeStr), "%02d:%02d:%02d",
             ti->tm_hour, ti->tm_min, ti->tm_sec);

    // Format date "2026年7月1日" (Chinese)
    snprintf(_data.dateStr, sizeof(_data.dateStr), "%04d年%d月%d日",
             ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday);

    // Weekday
    int wday = ti->tm_wday;  // 0=Sunday
    strncpy(_data.weekdayStr, _weekdayCN(wday), sizeof(_data.weekdayStr) - 1);
}

const char* TimeManager::_weekdayCN(int ntpDow) {
    static const char* days[] = {
        "星期日", "星期一", "星期二", "星期三",
        "星期四", "星期五", "星期六"
    };
    return days[ntpDow % 7];
}
