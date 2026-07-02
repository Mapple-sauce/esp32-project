#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <Arduino.h>
#include <Preferences.h>
#include "config.h"

#define ALARM_NVS_NS     "alarm"
#define ALARM_KEY_PREFIX "alm"
#define MAX_ALARMS       3

class AlarmManager {
public:
    AlarmManager();
    ~AlarmManager();

    // Load alarms from NVS, initialize
    void begin();

    // Check if any alarm should trigger right now
    // Returns -1 if none, or alarm index (0..MAX_ALARMS-1) if triggered
    int check(int hour, int minute, int weekday);

    // Getters
    AlarmConfig getAlarm(int index) const;
    int getEnabledCount() const;

    // Setters
    bool setAlarm(int index, const AlarmConfig& cfg);
    bool setAlarmTime(int index, uint8_t hour, uint8_t minute);
    bool setAlarmEnabled(int index, bool enabled);
    bool setAlarmRepeat(int index, int weekday, bool on);  // weekday 1=Mon..7=Sun
    bool setAlarmSnooze(int index, uint8_t minutes);

    // Toggle enabled/disabled
    bool toggleAlarm(int index);

    // Reset daily triggered flags (called on day change)
    void resetDailyFlags();

    // Mark an alarm as acknowledged (stop ringing)
    void acknowledgeAlarm(int index);

    // Check if alarm was already triggered today
    bool isTriggered(int index) const;

    // Debug: print all alarms to serial
    void printAll();

private:
    AlarmConfig _alarms[MAX_ALARMS];
    Preferences _prefs;
    int _lastCheckedDay;  // Used to detect day change (reset daily flags)

    // Load/save individual alarm to NVS
    void _loadAlarm(int index);
    void _saveAlarm(int index);

    // Set sensible defaults for a given alarm index
    void _defaultAlarm(int index);

    // Check if an alarm should fire today based on repeatDays
    bool _shouldFireToday(const AlarmConfig& cfg, int weekday) const;
};

#endif // ALARM_MANAGER_H
