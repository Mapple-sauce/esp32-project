#include "AlarmPage.h"

AlarmPage::AlarmPage()
    : _alarmMgr(nullptr)
    , _selectedAlarm(0)
    , _focus(ALARM_FOCUS_SELECT)
    , _editing(false)
    , _repeatFocusDay(0)
    , _forceRedraw(true)
{}

AlarmPage::~AlarmPage() {}

void AlarmPage::begin(AlarmManager* alarmMgr) {
    _alarmMgr = alarmMgr;
}

void AlarmPage::render(DisplayManager& display, int triggeredAlarm) {
    auto& tft = display.getTFT();
    int w = display.width();
    int h = display.height();
    int contentY = STATUS_BAR_H;

    if (_forceRedraw) {
        tft.fillRect(0, contentY, w, h - contentY, TFT_BLACK);
        _forceRedraw = false;
    } else {
        // Only redraw from content area
        tft.fillRect(0, contentY, w, h - contentY, TFT_BLACK);
    }

    // Title
    tft.setTextColor(TFT_ORANGE, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("⏰ Alarm Settings", w / 2, contentY + 15, 2);

    // Show all 3 alarm rows
    for (int i = 0; i < MAX_ALARMS; i++) {
        _drawAlarmRow(display, i, contentY + 50 + i * 40);
    }

    // If an alarm triggered, show notification
    if (triggeredAlarm >= 0) {
        tft.fillRoundRect(w / 2 - 80, h / 2 - 25, 160, 50, 8, TFT_MAROON);
        tft.setTextColor(TFT_WHITE, TFT_MAROON);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("⏰ ALARM!", w / 2, h / 2 - 8, 2);
        tft.drawString("Press MENU to stop", w / 2, h / 2 + 12, 1);
    }

    // Edit mode footer hint
    if (_editing) {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextDatum(BC_DATUM);
        tft.drawString("MENU:cycle  +/-:adjust", w / 2, h - 8, 1);
    }

    // Draw edit UI overlay if editing
    if (_editing) {
        _drawEditUI(display);
    }
}

void AlarmPage::_drawAlarmRow(DisplayManager& display, int index, int y) {
    auto& tft = display.getTFT();
    int w = display.width();
    AlarmConfig cfg = _alarmMgr->getAlarm(index);

    // Highlight selected alarm row
    uint16_t bg = (_editing && _selectedAlarm == index) ? TFT_NAVY : TFT_BLACK;
    tft.fillRect(10, y, w - 20, 35, bg);

    // Alarm label + enabled status
    char label[16];
    snprintf(label, sizeof(label), "Alarm %d", index + 1);
    tft.setTextColor(_selectedAlarm == index ? TFT_CYAN : TFT_WHITE, bg);
    tft.setTextDatum(ML_DATUM);
    tft.drawString(label, 15, y + 17, 1);

    // Enabled/disabled badge
    const char* status = cfg.enabled ? "ON" : "OFF";
    uint16_t statusColor = cfg.enabled ? TFT_GREEN : TFT_DARKGREY;
    tft.fillRoundRect(80, y + 8, 30, 18, 4, statusColor);
    tft.setTextColor(TFT_WHITE, statusColor);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(status, 95, y + 17, 1);

    // Time display
    char timeStr[8];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", cfg.hour, cfg.minute);
    tft.setTextColor(TFT_WHITE, bg);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(timeStr, w / 2 + 15, y + 17, 4);

    // Repeat indicator (compact)
    char repeatStr[16] = "";
    int repIdx = 0;
    for (int d = 0; d < 7 && repIdx < 7; d++) {
        if (cfg.repeatDays[d]) {
            repeatStr[repIdx++] = _dayLabel(d)[0];
        }
    }
    if (repIdx > 0) {
        repeatStr[repIdx] = '\0';
        tft.setTextColor(TFT_GREENYELLOW, bg);
        tft.setTextDatum(MR_DATUM);
        tft.drawString(repeatStr, w - 15, y + 17, 1);
    }
}

void AlarmPage::_drawEditUI(DisplayManager& display) {
    auto& tft = display.getTFT();
    int w = display.width();
    int contentY = STATUS_BAR_H;

    AlarmConfig cfg = _alarmMgr->getAlarm(_selectedAlarm);

    // Edit panel — overlay at bottom area
    int panelY = contentY + 170;
    int panelH = display.height() - panelY - 5;
    tft.fillRoundRect(5, panelY, w - 10, panelH, 6, TFT_DARKGREY);

    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Edit Alarm", w / 2, panelY + 8, 1);

    // Time editor row
    char editTime[16];
    snprintf(editTime, sizeof(editTime), "%s %02d : %02d",
             cfg.enabled ? "ON" : "OFF", cfg.hour, cfg.minute);

    int editY = panelY + 30;
    int xPos = 30;

    // ON/OFF
    tft.fillRoundRect(xPos, editY, 40, 24, 4, _focus == ALARM_FOCUS_ENABLE ? TFT_ORANGE : TFT_BLACK);
    tft.setTextColor(cfg.enabled ? TFT_GREEN : TFT_RED,
                     _focus == ALARM_FOCUS_ENABLE ? TFT_ORANGE : TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(cfg.enabled ? "ON" : "OFF", xPos + 20, editY + 12, 1);

    // Hours
    xPos += 50;
    tft.fillRoundRect(xPos, editY, 40, 24, 4, _focus == ALARM_FOCUS_HOUR ? TFT_ORANGE : TFT_BLACK);
    tft.setTextColor(TFT_WHITE, _focus == ALARM_FOCUS_HOUR ? TFT_ORANGE : TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(String(cfg.hour).c_str(), xPos + 20, editY + 12, 2);

    // Colon
    xPos += 45;
    tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    tft.drawString(":", xPos, editY + 12, 2);

    // Minutes
    xPos += 15;
    tft.fillRoundRect(xPos, editY, 40, 24, 4, _focus == ALARM_FOCUS_MINUTE ? TFT_ORANGE : TFT_BLACK);
    tft.setTextColor(TFT_WHITE, _focus == ALARM_FOCUS_MINUTE ? TFT_ORANGE : TFT_BLACK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(String(cfg.minute).c_str(), xPos + 20, editY + 12, 2);

    // Repeat days
    _drawRepeatDays(display, editY + 35, cfg);

    // Save / Cancel
    int btnY = panelY + panelH - 30;
    int btnW = 60;

    // Cancel
    tft.fillRoundRect(w / 2 - btnW - 15, btnY, btnW, 24, 4,
                      _focus == ALARM_FOCUS_DONE ? TFT_BLACK : TFT_DARKGREY);
    tft.setTextColor(TFT_WHITE, _focus == ALARM_FOCUS_DONE ? TFT_BLACK : TFT_DARKGREY);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Cancel", w / 2 - 15, btnY + 12, 1);

    // Done
    tft.fillRoundRect(w / 2 + 15, btnY, btnW, 24, 4,
                      _focus == ALARM_FOCUS_DONE ? TFT_ORANGE : TFT_BLUE);
    tft.setTextColor(TFT_WHITE, _focus == ALARM_FOCUS_DONE ? TFT_ORANGE : TFT_BLUE);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Save", w / 2 + 45, btnY + 12, 1);
}

void AlarmPage::_drawRepeatDays(DisplayManager& display, int y, const AlarmConfig& cfg) {
    auto& tft = display.getTFT();
    int w = display.width();
    int startX = (w - 7 * 30) / 2;

    tft.setTextColor(TFT_CYAN, TFT_DARKGREY);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Repeat:", w / 2, y - 2, 1);

    for (int d = 0; d < 7; d++) {
        int x = startX + d * 30;
        bool isFocused = (_focus == ALARM_FOCUS_REPEAT && _repeatFocusDay == d);
        bool isOn = cfg.repeatDays[d];

        uint16_t boxColor = isOn ? TFT_GREEN : (isFocused ? TFT_ORANGE : TFT_BLACK);
        tft.fillRoundRect(x, y + 10, 26, 18, 3, boxColor);
        tft.setTextColor(TFT_WHITE, boxColor);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(_dayLabel(d), x + 13, y + 19, 1);
    }
}

bool AlarmPage::handleKey(const KeyEvent& event) {
    if (!_alarmMgr) return false;

    // If not editing, entering edit mode on MENU short press
    if (!_editing) {
        if (event.button == BTN_ID_MENU && event.type == KEY_SHORT_PRESS) {
            _editing = true;
            _focus = ALARM_FOCUS_SELECT;
            _forceRedraw = true;
            return true;
        }
        return false;
    }

    // --- EDIT MODE ---
    switch (event.button) {
        case BTN_ID_MENU:
            if (event.type == KEY_SHORT_PRESS) {
                // Cycle focus
                _focus = (AlarmEditFocus)((_focus + 1) % ALARM_FOCUS_COUNT);
                _forceRedraw = true;
                return true;
            }
            if (event.type == KEY_LONG_PRESS) {
                // Long press = exit edit mode
                _editing = false;
                _forceRedraw = true;
                return true;
            }
            break;

        case BTN_ID_PLUS:
            if (event.type == KEY_SHORT_PRESS) {
                switch (_focus) {
                    case ALARM_FOCUS_SELECT:
                        _selectedAlarm = (_selectedAlarm + 1) % MAX_ALARMS;
                        break;
                    case ALARM_FOCUS_ENABLE:
                        _alarmMgr->toggleAlarm(_selectedAlarm);
                        break;
                    case ALARM_FOCUS_HOUR: {
                        AlarmConfig c = _alarmMgr->getAlarm(_selectedAlarm);
                        c.hour = (c.hour + 1) % 24;
                        _alarmMgr->setAlarm(_selectedAlarm, c);
                        break;
                    }
                    case ALARM_FOCUS_MINUTE: {
                        AlarmConfig c = _alarmMgr->getAlarm(_selectedAlarm);
                        c.minute = (c.minute + 1) % 60;
                        _alarmMgr->setAlarm(_selectedAlarm, c);
                        break;
                    }
                    case ALARM_FOCUS_REPEAT: {
                        // Toggle current day
                        AlarmConfig c = _alarmMgr->getAlarm(_selectedAlarm);
                        c.repeatDays[_repeatFocusDay] = !c.repeatDays[_repeatFocusDay];
                        _alarmMgr->setAlarm(_selectedAlarm, c);
                        break;
                    }
                    case ALARM_FOCUS_DONE:
                        // Save complete — exit edit
                        _editing = false;
                        _forceRedraw = true;
                        return true;
                }
                _forceRedraw = true;
            }
            break;

        case BTN_ID_MINUS:
            if (event.type == KEY_SHORT_PRESS) {
                switch (_focus) {
                    case ALARM_FOCUS_SELECT:
                        _selectedAlarm = (_selectedAlarm - 1 + MAX_ALARMS) % MAX_ALARMS;
                        break;
                    case ALARM_FOCUS_ENABLE:
                        _alarmMgr->toggleAlarm(_selectedAlarm);
                        break;
                    case ALARM_FOCUS_HOUR: {
                        AlarmConfig c = _alarmMgr->getAlarm(_selectedAlarm);
                        c.hour = (c.hour + 23) % 24;
                        _alarmMgr->setAlarm(_selectedAlarm, c);
                        break;
                    }
                    case ALARM_FOCUS_MINUTE: {
                        AlarmConfig c = _alarmMgr->getAlarm(_selectedAlarm);
                        c.minute = (c.minute + 59) % 60;
                        _alarmMgr->setAlarm(_selectedAlarm, c);
                        break;
                    }
                    case ALARM_FOCUS_REPEAT:
                        // Move to next day
                        _repeatFocusDay = (_repeatFocusDay + 1) % 7;
                        _forceRedraw = true;
                        return true;
                    case ALARM_FOCUS_DONE:
                        // Save complete — exit edit
                        _editing = false;
                        _forceRedraw = true;
                        return true;
                }
                _forceRedraw = true;
            }
            break;

        default:
            break;
    }

    return true;
}

const char* AlarmPage::_dayLabel(int idx) const {
    static const char* labels[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    if (idx < 0 || idx > 6) return "?";
    return labels[idx];
}
