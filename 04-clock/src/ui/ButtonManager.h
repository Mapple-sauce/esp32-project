#ifndef BUTTON_MANAGER_H
#define BUTTON_MANAGER_H

#include <Arduino.h>
#include "config.h"

// Button identifiers
enum ButtonID {
    BTN_ID_NONE = -1,
    BTN_ID_MENU = 0,
    BTN_ID_PLUS = 1,
    BTN_ID_MINUS = 2,
};

// Key event types
enum KeyEventType {
    KEY_NONE = 0,
    KEY_SHORT_PRESS,    // < 1000ms
    KEY_LONG_PRESS,     // >= 1000ms
};

// A single key event
struct KeyEvent {
    ButtonID button;
    KeyEventType type;
};

class ButtonManager {
public:
    ButtonManager();
    ~ButtonManager();

    // Initialize button pins (INPUT_PULLUP)
    void begin();

    // Scan all buttons — call every loop()
    // Returns events detected since last scan via output array
    int scan(KeyEvent* events, int maxEvents);

    // Direct button state queries
    bool isPressed(ButtonID btn) const;
    bool isMenuPressed()    const { return isPressed(BTN_ID_MENU); }
    bool isPlusPressed()    const { return isPressed(BTN_ID_PLUS); }
    bool isMinusPressed()   const { return isPressed(BTN_ID_MINUS); }

    // Get the raw GPIO level (for debug)
    int readRaw(ButtonID btn) const;

private:
    struct DebouncedButton {
        uint8_t pin;
        bool lastReading;       // Last raw reading (HIGH = released)
        bool currentState;      // Debounced state (true = pressed LOW)
        unsigned long lastChangeMs;
        unsigned long pressStartMs;
        bool longPressReported;
    };

    DebouncedButton _buttons[3];

    void _setupPin(DebouncedButton& btn, uint8_t pin);
    void _scanOne(DebouncedButton& btn, ButtonID id, KeyEvent* events, int& count, int maxEvents);
};

#endif // BUTTON_MANAGER_H
