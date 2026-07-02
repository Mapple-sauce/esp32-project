#include "TimerPage.h"

TimerPage::TimerPage()
    : _state(TIMER_IDLE)
    , _totalSeconds(900)       // Default 15:00
    , _remainingSeconds(900)
    , _lastTickMs(0)
    , _presetIndex(2)          // 15 min default
    , _forceRedraw(true)
    , _justFinished(false)
{}

TimerPage::~TimerPage() {}

void TimerPage::begin() {
    _totalSeconds = TIMER_PRESETS[_presetIndex];
    _remainingSeconds = _totalSeconds;
}

void TimerPage::reset() {
    _state = TIMER_IDLE;
    _totalSeconds = TIMER_PRESETS[_presetIndex];
    _remainingSeconds = _totalSeconds;
    _lastTickMs = 0;
    _justFinished = false;
    _forceRedraw = true;
}

void TimerPage::update() {
    if (_state != TIMER_RUNNING) return;

    unsigned long now = millis();
    // Decrement every second
    if (now - _lastTickMs >= 1000) {
        if (_remainingSeconds > 0) {
            _remainingSeconds--;
            _lastTickMs = now;
        }

        if (_remainingSeconds == 0) {
            _state = TIMER_FINISHED;
            _justFinished = true;
            _forceRedraw = true;
        }
    }
}

void TimerPage::render(DisplayManager& display) {
    auto& tft = display.getTFT();
    int w = display.width();
    int h = display.height();
    int contentY = STATUS_BAR_H;

    if (_forceRedraw) {
        tft.fillRect(0, contentY, w, h - contentY, TFT_BLACK);
        _forceRedraw = false;
    } else {
        // Efficient: only update what changed
        // TODO: partial redraw optimization
        tft.fillRect(0, contentY, w, h - contentY, TFT_BLACK);
    }

    // Title
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("⏱ Timer", w / 2, contentY + 15, 2);

    // State label
    tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(stateLabel(_state), w / 2, contentY + 38, 1);

    // Large countdown digits
    _drawTimerDigits(display, contentY + 55);

    // Preset strip (in idle mode, show presets)
    if (_state == TIMER_IDLE) {
        _drawPresets(display, contentY + 140);
    }

    // Control buttons
    _drawControls(display, contentY + 175);
}

void TimerPage::_drawTimerDigits(DisplayManager& display, int y) {
    auto& tft = display.getTFT();
    int w = display.width();

    char timeStr[8];
    _formatTime(timeStr, sizeof(timeStr), _remainingSeconds);

    uint16_t color;
    switch (_state) {
        case TIMER_IDLE:     color = TFT_WHITE; break;
        case TIMER_RUNNING:  color = (_remainingSeconds <= 10) ? TFT_RED : TFT_GREEN; break;
        case TIMER_PAUSED:   color = TFT_YELLOW; break;
        case TIMER_FINISHED: color = TFT_RED; break;
        default:             color = TFT_WHITE;
    }

    tft.setTextColor(color, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(timeStr, w / 2, y, 7);
}

void TimerPage::_drawPresets(DisplayManager& display, int y) {
    auto& tft = display.getTFT();
    int w = display.width();

    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Preset:", w / 2, y, 1);

    int spacing = (w - 20) / TIMER_PRESET_COUNT;
    int startX = 10;

    for (int i = 0; i < (int)TIMER_PRESET_COUNT; i++) {
        int x = startX + i * spacing;
        bool selected = (i == _presetIndex);
        uint32_t secs = TIMER_PRESETS[i];

        char label[8];
        if (secs < 60) {
            snprintf(label, sizeof(label), "%dS", secs);
        } else if (secs < 3600) {
            snprintf(label, sizeof(label), "%dM", secs / 60);
        } else {
            snprintf(label, sizeof(label), "%dH", secs / 3600);
        }

        uint16_t bg = selected ? TFT_BLUE : TFT_DARKGREY;
        tft.fillRoundRect(x, y + 12, spacing - 4, 22, 4, bg);
        tft.setTextColor(TFT_WHITE, bg);
        tft.setTextDatum(MC_DATUM);
        tft.drawString(label, x + (spacing - 4) / 2, y + 23, 1);
    }
}

void TimerPage::_drawControls(DisplayManager& display, int y) {
    auto& tft = display.getTFT();
    int w = display.width();
    int btnW = 60;

    switch (_state) {
        case TIMER_IDLE: {
            // [Start] button
            tft.fillRoundRect(w / 2 - btnW / 2, y, btnW, 30, 6, TFT_GREEN);
            tft.setTextColor(TFT_WHITE, TFT_GREEN);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("▶ Start", w / 2, y + 15, 1);
            break;
        }
        case TIMER_RUNNING: {
            // [Pause] [Reset]
            tft.fillRoundRect(w / 2 - btnW - 10, y, btnW, 30, 6, TFT_ORANGE);
            tft.setTextColor(TFT_WHITE, TFT_ORANGE);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("⏸ Pause", w / 2 - btnW / 2 - 10, y + 15, 1);

            tft.fillRoundRect(w / 2 + 10, y, btnW, 30, 6, TFT_DARKGREY);
            tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("↺ Reset", w / 2 + btnW / 2 + 10, y + 15, 1);
            break;
        }
        case TIMER_PAUSED: {
            // [Resume] [Reset]
            tft.fillRoundRect(w / 2 - btnW - 10, y, btnW, 30, 6, TFT_GREEN);
            tft.setTextColor(TFT_WHITE, TFT_GREEN);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("▶ Resume", w / 2 - btnW / 2 - 10, y + 15, 1);

            tft.fillRoundRect(w / 2 + 10, y, btnW, 30, 6, TFT_RED);
            tft.setTextColor(TFT_WHITE, TFT_RED);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("↺ Reset", w / 2 + btnW / 2 + 10, y + 15, 1);
            break;
        }
        case TIMER_FINISHED: {
            // [Done] button (reset)
            tft.fillRoundRect(w / 2 - btnW / 2, y, btnW, 30, 6, TFT_RED);
            tft.setTextColor(TFT_WHITE, TFT_RED);
            tft.setTextDatum(MC_DATUM);
            tft.drawString("✕ Done", w / 2, y + 15, 1);
            break;
        }
    }
}

bool TimerPage::handleKey(const KeyEvent& event) {
    if (event.type != KEY_SHORT_PRESS) return false;

    // TIMER_FINISHED: any key resets
    if (_state == TIMER_FINISHED) {
        reset();
        return true;
    }

    switch (event.button) {
        case BTN_ID_MENU:
            // MENU: in IDLE cycle presets; in RUNNING/PAUSED = reset
            if (_state == TIMER_IDLE) {
                _presetIndex = (_presetIndex + 1) % TIMER_PRESET_COUNT;
                _totalSeconds = TIMER_PRESETS[_presetIndex];
                _remainingSeconds = _totalSeconds;
                _forceRedraw = true;
            } else if (_state == TIMER_RUNNING || _state == TIMER_PAUSED) {
                reset();
            }
            return true;

        case BTN_ID_PLUS:
            // PLUS: start (idle) or resume (paused)
            if (_state == TIMER_IDLE) {
                _state = TIMER_RUNNING;
                _lastTickMs = millis();
                _forceRedraw = true;
            } else if (_state == TIMER_PAUSED) {
                _state = TIMER_RUNNING;
                _lastTickMs = millis();
                _forceRedraw = true;
            }
            return true;

        case BTN_ID_MINUS:
            // MINUS: pause (running) or reset (paused/idle)
            if (_state == TIMER_RUNNING) {
                _state = TIMER_PAUSED;
                _forceRedraw = true;
            } else if (_state == TIMER_PAUSED || _state == TIMER_IDLE) {
                reset();
            }
            return true;

        default:
            break;
    }
    return false;
}

void TimerPage::_formatTime(char* buf, size_t len, uint32_t seconds) const {
    uint32_t minutes = seconds / 60;
    uint32_t secs = seconds % 60;
    snprintf(buf, len, "%02u:%02u", minutes, secs);
}

const char* TimerPage::stateLabel(TimerState s) {
    switch (s) {
        case TIMER_IDLE:     return "Ready";
        case TIMER_RUNNING:  return "Running";
        case TIMER_PAUSED:   return "Paused";
        case TIMER_FINISHED: return "Time's Up!";
        default:             return "";
    }
}
