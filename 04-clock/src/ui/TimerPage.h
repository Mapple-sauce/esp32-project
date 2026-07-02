#ifndef TIMER_PAGE_H
#define TIMER_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"
#include "ButtonManager.h"

// Timer states
enum TimerState {
    TIMER_IDLE = 0,
    TIMER_RUNNING,
    TIMER_PAUSED,
    TIMER_FINISHED,
};

// Preset durations (in seconds)
static const uint32_t TIMER_PRESETS[] = {60, 300, 900, 1800, 3600};
#define TIMER_PRESET_COUNT (sizeof(TIMER_PRESETS) / sizeof(TIMER_PRESETS[0]))

class TimerPage {
public:
    TimerPage();
    ~TimerPage();

    void begin();

    // Render the timer page
    void render(DisplayManager& display);

    // Handle button events
    // Returns true if consumed
    bool handleKey(const KeyEvent& event);

    // Call every loop() for non-blocking countdown
    void update();

    // Check if timer just finished (for buzzer notification)
    bool isFinished() const { return _state == TIMER_FINISHED; }
    bool justFinished() { bool jf = _justFinished; _justFinished = false; return jf; }

    // Force redraw
    void forceRedraw() { _forceRedraw = true; }

    // Reset timer
    void reset();

    // Get remaining seconds for display
    uint32_t getRemaining() const { return _remainingSeconds; }
    TimerState getState() const { return _state; }

    // Labels for UI
    static const char* stateLabel(TimerState s);

private:
    TimerState _state;
    uint32_t _totalSeconds;
    uint32_t _remainingSeconds;
    unsigned long _lastTickMs;
    int _presetIndex;       // Which preset is selected (idle mode)
    bool _forceRedraw;
    bool _justFinished;     // Set when timer first enters FINISHED state

    // Format MM:SS for display
    void _formatTime(char* buf, size_t len, uint32_t seconds) const;

    // Draw the large countdown digits
    void _drawTimerDigits(DisplayManager& display, int y);
    // Draw preset bar
    void _drawPresets(DisplayManager& display, int y);
    // Draw control buttons
    void _drawControls(DisplayManager& display, int y);
};

#endif // TIMER_PAGE_H
