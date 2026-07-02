#include "AlarmManager.h"

AlarmManager::AlarmManager()
    : _lastCheckedDay(-1)
{
    for (int i = 0; i < MAX_ALARMS; i++) {
        memset(&_alarms[i], 0, sizeof(AlarmConfig));
        _alarms[i].snoozeMinutes = SNOOZE_MINUTES;
    }
}

AlarmManager::~AlarmManager() {
    _prefs.end();
}

void AlarmManager::begin() {
    _prefs.begin(ALARM_NVS_NS, false);
    for (int i = 0; i < MAX_ALARMS; i++) {
        _loadAlarm(i);
    }
    Serial.printf("[Alarm] Loaded %d alarms from NVS\n", MAX_ALARMS);

    // Detect today's date for daily reset
    struct tm ti;
    if (getLocalTime(&ti)) {
        _lastCheckedDay = ti.tm_mday;
    }
}

int AlarmManager::check(int hour, int minute, int weekday) {
    // Detect day change → reset triggered flags
    struct tm ti;
    if (getLocalTime(&ti) && ti.tm_mday != _lastCheckedDay) {
        if (_lastCheckedDay > 0) {
            resetDailyFlags();
        }
        _lastCheckedDay = ti.tm_mday;
    }

    // Check each alarm
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (!_alarms[i].enabled) continue;
        if (_alarms[i].triggered) continue;

        if (_alarms[i].hour == hour && _alarms[i].minute == minute) {
            if (_shouldFireToday(_alarms[i], weekday)) {
                _alarms[i].triggered = true;
                _saveAlarm(i);
                Serial.printf("[Alarm] Alarm %d triggered at %02d:%02d!\n",
                              i, hour, minute);
                return i;
            }
        }
    }

    return -1;
}

bool AlarmManager::_shouldFireToday(const AlarmConfig& cfg, int weekday) const {
    // weekday: 1=Monday ... 7=Sunday
    // repeatDays[0]=Mon ... repeatDays[6]=Sun
    bool anyRepeat = false;
    for (int d = 0; d < 7; d++) {
        if (cfg.repeatDays[d]) { anyRepeat = true; break; }
    }

    // No repeat days set → one-time alarm (fire today)
    if (!anyRepeat) return true;

    // Check if today is a repeat day
    int idx = weekday - 1;  // convert 1-based to 0-based
    if (idx < 0 || idx >= 7) return false;
    return cfg.repeatDays[idx];
}

AlarmConfig AlarmManager::getAlarm(int index) const {
    if (index < 0 || index >= MAX_ALARMS) {
        AlarmConfig empty;
        memset(&empty, 0, sizeof(AlarmConfig));
        return empty;
    }
    return _alarms[index];
}

int AlarmManager::getEnabledCount() const {
    int count = 0;
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (_alarms[i].enabled) count++;
    }
    return count;
}

bool AlarmManager::setAlarm(int index, const AlarmConfig& cfg) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    _alarms[index] = cfg;
    _saveAlarm(index);
    return true;
}

bool AlarmManager::setAlarmTime(int index, uint8_t hour, uint8_t minute) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    _alarms[index].hour = hour;
    _alarms[index].minute = minute;
    _saveAlarm(index);
    return true;
}

bool AlarmManager::setAlarmEnabled(int index, bool enabled) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    _alarms[index].enabled = enabled;
    if (!enabled) _alarms[index].triggered = false;  // Reset trigger when disabled
    _saveAlarm(index);
    return true;
}

bool AlarmManager::setAlarmRepeat(int index, int weekday, bool on) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    if (weekday < 1 || weekday > 7) return false;
    _alarms[index].repeatDays[weekday - 1] = on;
    _saveAlarm(index);
    return true;
}

bool AlarmManager::setAlarmSnooze(int index, uint8_t minutes) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    _alarms[index].snoozeMinutes = minutes;
    _saveAlarm(index);
    return true;
}

bool AlarmManager::toggleAlarm(int index) {
    if (index < 0 || index >= MAX_ALARMS) return false;
    _alarms[index].enabled = !_alarms[index].enabled;
    if (!_alarms[index].enabled) _alarms[index].triggered = false;
    _saveAlarm(index);
    return true;
}

void AlarmManager::resetDailyFlags() {
    for (int i = 0; i < MAX_ALARMS; i++) {
        if (_alarms[i].triggered) {
            _alarms[i].triggered = false;
            _saveAlarm(i);
        }
    }
    Serial.println("[Alarm] Daily flags reset");
}

void AlarmManager::acknowledgeAlarm(int index) {
    if (index < 0 || index >= MAX_ALARMS) return;
    _alarms[index].triggered = true;  // Mark so it doesn't re-trigger today
    _saveAlarm(index);
}

bool AlarmManager::isTriggered(int index) const {
    if (index < 0 || index >= MAX_ALARMS) return false;
    return _alarms[index].triggered;
}

void AlarmManager::printAll() {
    Serial.println("[Alarm] Current alarms:");
    for (int i = 0; i < MAX_ALARMS; i++) {
        auto& a = _alarms[i];
        Serial.printf("  [%d] %s %02d:%02d snooze=%dmin triggered=%s\n",
                      i, a.enabled ? "ENABLED" : "DISABLED",
                      a.hour, a.minute, a.snoozeMinutes,
                      a.triggered ? "YES" : "NO");
        Serial.print("       Repeat: ");
        for (int d = 0; d < 7; d++) {
            Serial.print(a.repeatDays[d] ? "Y " : "- ");
        }
        Serial.println();
    }
}

void AlarmManager::_loadAlarm(int index) {
    char key[8];
    snprintf(key, sizeof(key), "%s%d", ALARM_KEY_PREFIX, index);

    size_t sz = sizeof(AlarmConfig);
    if (_prefs.isKey(key) && _prefs.getBytesLength(key) == sz) {
        _prefs.getBytes(key, &_alarms[index], sz);
    } else {
        _defaultAlarm(index);
        _saveAlarm(index);
    }
}

void AlarmManager::_saveAlarm(int index) {
    char key[8];
    snprintf(key, sizeof(key), "%s%d", ALARM_KEY_PREFIX, index);
    _prefs.putBytes(key, &_alarms[index], sizeof(AlarmConfig));
}

void AlarmManager::_defaultAlarm(int index) {
    memset(&_alarms[index], 0, sizeof(AlarmConfig));
    _alarms[index].enabled = false;
    _alarms[index].hour = 7 + index;   // 07:00, 08:00, 09:00
    _alarms[index].minute = 30;
    _alarms[index].snoozeMinutes = SNOOZE_MINUTES;
    _alarms[index].triggered = false;
    // No repeat days = one-time alarm
}
