#include "Buzzer.h"

Buzzer::Buzzer()
    : _pin(0)
    , _active(false)
    , _currentPattern(BUZZER_OFF)
    , _patternStartMs(0)
    , _lastToggleMs(0)
    , _soundOn(false)
    , _beepCount(0)
{}

Buzzer::~Buzzer() {
    stop();
}

void Buzzer::begin(uint8_t pin) {
    _pin = pin;
    #if BUZZER_AVAILABLE
    ledcSetup(BUZZER_CHANNEL, BUZZER_FREQ, BUZZER_RES);
    ledcAttachPin(_pin, BUZZER_CHANNEL);
    ledcWrite(BUZZER_CHANNEL, 0);
    #else
    (void)_pin;
    #endif
}

void Buzzer::play(BuzzerPattern pattern) {
    #if !BUZZER_AVAILABLE
    (void)pattern;
    #endif
    if (pattern == BUZZER_OFF) {
        stop();
        return;
    }
    _currentPattern = pattern;
    _active = true;
    _patternStartMs = millis();
    _lastToggleMs = millis();
    _soundOn = false;
    _beepCount = 0;
    _beginPlay();
}

void Buzzer::stop() {
    #if BUZZER_AVAILABLE
    _setOutput(false);
    #endif
    _active = false;
    _currentPattern = BUZZER_OFF;
}

void Buzzer::_setOutput(bool on) {
    #if BUZZER_AVAILABLE
    if (on) {
        ledcWrite(BUZZER_CHANNEL, 128);
    } else {
        ledcWrite(BUZZER_CHANNEL, 0);
    }
    #else
    (void)on;
    #endif
}

void Buzzer::_beginPlay() {
    #if !BUZZER_AVAILABLE
    return;
    #endif
    unsigned long now = millis();
    switch (_currentPattern) {
        case BUZZER_KEY_CLICK:
            _setOutput(true);
            _lastToggleMs = now;
            break;
        case BUZZER_ALARM:
            _setOutput(true);
            _soundOn = true;
            _lastToggleMs = now;
            break;
        case BUZZER_TIMER_DONE:
            _setOutput(true);
            _soundOn = true;
            _lastToggleMs = now;
            break;
        case BUZZER_NOTIFICATION:
            _setOutput(true);
            _soundOn = true;
            _beepCount = 0;
            _lastToggleMs = now;
            break;
        default:
            break;
    }
}

void Buzzer::update() {
    if (!_active) return;

    unsigned long now = millis();

    switch (_currentPattern) {
        case BUZZER_KEY_CLICK:
            if (now - _lastToggleMs >= 50) {
                stop();
            }
            break;
        case BUZZER_ALARM:
            if (now - _lastToggleMs >= 200) {
                _soundOn = !_soundOn;
                _setOutput(_soundOn);
                _lastToggleMs = now;
            }
            if (now - _patternStartMs >= 60000) {
                stop();
            }
            break;
        case BUZZER_TIMER_DONE:
            if (now - _patternStartMs >= 2000) {
                stop();
            }
            break;
        case BUZZER_NOTIFICATION:
            if (_beepCount == 0) {
                if (now - _lastToggleMs >= 100) {
                    _setOutput(false);
                    _beepCount = 1;
                    _lastToggleMs = now;
                }
            } else if (_beepCount == 1) {
                if (now - _lastToggleMs >= 100) {
                    _setOutput(true);
                    _beepCount = 2;
                    _lastToggleMs = now;
                }
            } else if (_beepCount == 2) {
                if (now - _lastToggleMs >= 100) {
                    stop();
                }
            }
            break;
        default:
            break;
    }
}

void Buzzer::click() {
    #if BUZZER_AVAILABLE
    _setOutput(true);
    delay(50);
    _setOutput(false);
    #endif
}
