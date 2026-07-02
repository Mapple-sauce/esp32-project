#include "ButtonManager.h"

ButtonManager::ButtonManager() {
    _buttons[0].pin = 0; _buttons[1].pin = 0; _buttons[2].pin = 0;
    _buttons[0].lastReading = HIGH;
    _buttons[1].lastReading = HIGH;
    _buttons[2].lastReading = HIGH;
    _buttons[0].currentState = HIGH;
    _buttons[1].currentState = HIGH;
    _buttons[2].currentState = HIGH;
    _buttons[0].lastChangeMs = 0;
    _buttons[1].lastChangeMs = 0;
    _buttons[2].lastChangeMs = 0;
    _buttons[0].pressStartMs = 0;
    _buttons[1].pressStartMs = 0;
    _buttons[2].pressStartMs = 0;
    _buttons[0].longPressReported = false;
    _buttons[1].longPressReported = false;
    _buttons[2].longPressReported = false;
}

ButtonManager::~ButtonManager() {}

void ButtonManager::begin() {
    _setupPin(_buttons[BTN_ID_MENU], BTN_MENU);
    _setupPin(_buttons[BTN_ID_PLUS], BTN_PLUS);
    _setupPin(_buttons[BTN_ID_MINUS], BTN_MINUS);
    Serial.println("[Button] MENU=13 PLUS=14 MINUS=27 (INPUT_PULLUP)");
}

void ButtonManager::_setupPin(DebouncedButton& btn, uint8_t pin) {
    btn.pin = pin;
    pinMode(pin, INPUT_PULLUP);
    btn.lastReading = digitalRead(pin);
    btn.currentState = btn.lastReading;
    btn.lastChangeMs = millis();
    btn.pressStartMs = 0;
    btn.longPressReported = false;
}

int ButtonManager::scan(KeyEvent* events, int maxEvents) {
    int count = 0;
    _scanOne(_buttons[BTN_ID_MENU],  BTN_ID_MENU,  events, count, maxEvents);
    _scanOne(_buttons[BTN_ID_PLUS],  BTN_ID_PLUS,  events, count, maxEvents);
    _scanOne(_buttons[BTN_ID_MINUS], BTN_ID_MINUS, events, count, maxEvents);
    return count;
}

void ButtonManager::_scanOne(DebouncedButton& btn, ButtonID id,
                              KeyEvent* events, int& count, int maxEvents) {
    unsigned long now = millis();
    bool reading = digitalRead(btn.pin);

    // Debounce: only change state after stable reading
    if (reading != btn.lastReading) {
        btn.lastChangeMs = now;
    }

    if ((now - btn.lastChangeMs) > DEBOUNCE_MS) {
        // Stable reading differs from current state?
        if (reading != btn.currentState) {
            btn.currentState = reading;
            btn.pressStartMs = reading ? 0 : now;  // record press start (LOW = pressed)
            btn.longPressReported = false;

            if (reading) {
                // Button released — check for short press
                if (now - btn.pressStartMs < LONG_PRESS_MS && btn.pressStartMs > 0) {
                    if (count < maxEvents) {
                        events[count].button = id;
                        events[count].type = KEY_SHORT_PRESS;
                        count++;
                    }
                }
            }
        }
    }

    // Check for long press while held
    if (!btn.currentState && !btn.longPressReported) {
        if ((now - btn.pressStartMs) >= LONG_PRESS_MS) {
            btn.longPressReported = true;
            if (count < maxEvents) {
                events[count].button = id;
                events[count].type = KEY_LONG_PRESS;
                count++;
            }
        }
    }

    btn.lastReading = reading;
}

bool ButtonManager::isPressed(ButtonID btn) const {
    if (btn < 0 || btn > 2) return false;
    return !_buttons[btn].currentState;  // LOW = pressed
}

int ButtonManager::readRaw(ButtonID btn) const {
    if (btn < 0 || btn > 2) return HIGH;
    return digitalRead(_buttons[btn].pin);
}
