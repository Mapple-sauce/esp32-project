#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "config.h"

class TimeManager {
public:
    TimeManager();
    ~TimeManager();

    // Initialize NTP client
    void begin();

    // Update from NTP (call periodically)
    bool update();

    // Get current time as a formatted TimeData struct
    TimeData getTimeData();

    // Get formatted time string (for status bar)
    const char* getTimeString();

    // Get formatted date string (Chinese)
    const char* getDateString();

    // Get weekday string (Chinese)
    const char* getWeekdayString();

    // Check if time has been synced
    bool isTimeValid() const { return _timeValid; }

    // Manual set time (for debugging / offline mode)
    void setManual(int year, int month, int day, int hour, int min, int sec);

    // Seconds since last NTP sync
    unsigned long getUptime() const;

private:
    WiFiUDP _udp;
    NTPClient _ntp;
    bool _timeValid;
    unsigned long _lastSyncMs;

    // Cached formatted strings
    TimeData _data;

    // Update cached strings from current time
    void _updateStrings();

    // Convert NTP weekday (0=Sunday) to Chinese (1=Monday...7=Sunday)
    static const char* _weekdayCN(int ntpDow);
};

#endif // TIME_MANAGER_H
