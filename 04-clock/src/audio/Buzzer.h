#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
#include "config.h"

// Sound patterns
enum BuzzerPattern {
    BUZZER_OFF = 0,
    BUZZER_KEY_CLICK,      // 50ms short beep
    BUZZER_ALARM,          // Intermittent 200ms on/off
    BUZZER_TIMER_DONE,     // Continuous 1s tone
    BUZZER_NOTIFICATION,   // Two short beeps
};

class Buzzer {
public:
    Buzzer();
    ~Buzzer();

    // Initialize PWM
    void begin(uint8_t pin = BUZZER_PIN);

    // Start a non-blocking pattern playback
    void play(BuzzerPattern pattern);

    // Stop immediately
    void stop();

    // Call in loop() for pattern timing
    void update();

    // Blocking short tone (for boot/feedback, <200ms)
    void click();

    bool isPlaying() const { return _active; }
    BuzzerPattern getCurrentPattern() const { return _currentPattern; }

private:
    uint8_t _pin;
    bool _active;
    BuzzerPattern _currentPattern;
    unsigned long _patternStartMs;
    unsigned long _lastToggleMs;
    bool _soundOn;
    int _beepCount;        // For multi-beep patterns

    void _setOutput(bool on);
    void _beginPlay();
};

#endif // BUZZER_H
