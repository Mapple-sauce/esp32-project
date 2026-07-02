#ifndef ALARM_PAGE_H
#define ALARM_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"
#include "../time/AlarmManager.h"
#include "ButtonManager.h"

// Edit focus states
enum AlarmEditFocus {
    ALARM_FOCUS_SELECT = 0,  // Select which alarm
    ALARM_FOCUS_ENABLE,      // Enable/disable toggle
    ALARM_FOCUS_HOUR,        // Hour digits
    ALARM_FOCUS_MINUTE,      // Minute digits
    ALARM_FOCUS_REPEAT,      // Repeat day selection
    ALARM_FOCUS_DONE,        // Done button
    ALARM_FOCUS_COUNT
};

class AlarmPage {
public:
    AlarmPage();
    ~AlarmPage();

    void begin(AlarmManager* alarmMgr);

    // Render the alarm page
    void render(DisplayManager& display, int triggeredAlarm);

    // Handle a key event from ButtonManager
    // Returns true if the event was consumed
    bool handleKey(const KeyEvent& event);

    // Force redraw on next render
    void forceRedraw() { _forceRedraw = true; }

    // Get current edit state
    bool isEditing() const { return _editing; }

    // Exit edit mode (e.g., when page is auto-switched away)
    void exitEdit() { _editing = false; }

private:
    AlarmManager* _alarmMgr;
    int _selectedAlarm;          // Which alarm index is being edited
    AlarmEditFocus _focus;       // Which field is focused
    bool _editing;               // In edit mode?
    bool _repeatFocusDay;        // Which repeat day index (0-6) is focused
    bool _forceRedraw;

    // Draw alarm row
    void _drawAlarmRow(DisplayManager& display, int index, int y);
    // Draw the edit interface
    void _drawEditUI(DisplayManager& display);
    // Draw repeat day row
    void _drawRepeatDays(DisplayManager& display, int y, const AlarmConfig& cfg);

    // Helper: draw the focused field indicator (inverted / highlight)
    void _drawFocusHighlight(DisplayManager& display, int x, int y, int w, int h, bool focused);

    // Weekday short labels
    const char* _dayLabel(int idx) const;  // 0=Mon...6=Sun
};

#endif // ALARM_PAGE_H
