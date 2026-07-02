#include "DHT22Sensor.h"

DHT22Sensor::DHT22Sensor()
    : _dht(DHT22_PIN, DHT22)
    , _lastReadMs(0)
    , _pin(DHT22_PIN)
    , _errorCount(0)
{
    _data.temperature = 0;
    _data.humidity = 0;
    _data.valid = false;
    _data.lastUpdate = 0;
}

DHT22Sensor::~DHT22Sensor() {}

bool DHT22Sensor::begin(uint8_t pin) {
    _pin = pin;
    _dht.setup(pin, DHT22);
    delay(1000);  // DHT22 needs 1s after power-up

    // Test read
    float t = _dht.getTemperature();
    float h = _dht.getHumidity();

    if (isnan(t) || isnan(h)) {
        Serial.println("[DHT22] Initial read failed (may need warm-up)");
        // Not fatal — DHT22 sometimes needs 2s after power
        delay(1000);
        t = _dht.getTemperature();
        h = _dht.getHumidity();
    }

    if (!isnan(t) && !isnan(h)) {
        _data.temperature = t;
        _data.humidity = h;
        _data.valid = true;
        _data.lastUpdate = millis();
        _errorCount = 0;
        Serial.printf("[DHT22] OK — %.1f°C, %.1f%%\n", t, h);
        return true;
    }

    Serial.println("[DHT22] Init failed after retry");
    return false;
}

bool DHT22Sensor::read() {
    float t = _dht.getTemperature();
    float h = _dht.getHumidity();

    if (isnan(t) || isnan(h)) {
        _errorCount++;
        Serial.printf("[DHT22] Read error (%d/%d)\n", _errorCount, MAX_ERRORS);

        if (_errorCount >= MAX_ERRORS) {
            _data.valid = false;
        }
        return false;
    }

    _data.temperature = t;
    _data.humidity = h;
    _data.valid = true;
    _data.lastUpdate = millis();
    _errorCount = 0;
    return true;
}

bool DHT22Sensor::isHealthy() const {
    if (!_data.valid) return false;
    // Data is stale if not updated in 30 seconds
    if (millis() - _data.lastUpdate > 30000) return false;
    return _errorCount < MAX_ERRORS;
}
