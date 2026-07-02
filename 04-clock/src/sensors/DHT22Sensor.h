#ifndef DHT22_SENSOR_H
#define DHT22_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "config.h"

class DHT22Sensor {
public:
    DHT22Sensor();
    ~DHT22Sensor();

    // Initialize sensor
    bool begin(uint8_t pin = DHT22_PIN);

    // Read sensor data (call periodically)
    bool read();

    // Get latest data
    IndoorData getData() const { return _data; }

    // Temperature in °C
    float getTemperature() const { return _data.temperature; }

    // Humidity in %
    float getHumidity() const { return _data.humidity; }

    // Check if reading was successful recently
    bool isHealthy() const;

private:
    DHT _dht;
    IndoorData _data;
    unsigned long _lastReadMs;
    uint8_t _pin;
    int _errorCount;       // Consecutive read failures
    static const int MAX_ERRORS = 5;
};

#endif // DHT22_SENSOR_H
