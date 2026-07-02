#include "WeatherClient.h"

WeatherClient::WeatherClient()
    : _lastFetchMs(0)
{
    memset(&_data, 0, sizeof(_data));
    strncpy(_location, QWEATHER_CITY_ID, sizeof(_location) - 1);
}

WeatherClient::~WeatherClient() {}

void WeatherClient::setLocation(const char* cityId) {
    strncpy(_location, cityId, sizeof(_location) - 1);
}

bool WeatherClient::fetch() {
    HTTPClient http;
    String url = String("http://devapi.qweather.com/v7/weather/now?location=")
                 + _location + "&key=" + QWEATHER_API_KEY;

    http.begin(url);
    http.setConnectTimeout(5000);
    http.setTimeout(8000);
    http.addHeader("User-Agent", "ESP32-Ornament/1.0");

    int httpCode = http.GET();
    if (httpCode <= 0) {
        Serial.printf("[Weather] HTTP error: %s\n", http.errorToString(httpCode).c_str());
        http.end();
        return false;
    }

    if (httpCode != 200) {
        Serial.printf("[Weather] HTTP %d\n", httpCode);
        http.end();
        return false;
    }

    String payload = http.getString();
    http.end();

    if (!_parseResponse(payload)) return false;

    _data.valid = true;
    _data.lastUpdate = millis();
    _lastFetchMs = millis();

    Serial.printf("[Weather] %s, %.1f°C, %d%% RH, wind %d scale\n",
                  _data.condition, _data.temp, _data.humidity, _data.windScale);
    return true;
}

bool WeatherClient::isDataFresh() const {
    if (!_data.valid) return false;
    if (millis() - _data.lastUpdate > QWEATHER_UPDATE_INTERVAL_MS + 60000) return false;
    return true;
}

bool WeatherClient::_parseResponse(const String& json) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        Serial.printf("[Weather] JSON error: %s\n", err.c_str());
        return false;
    }

    int code = doc["code"] | -1;
    if (code != 200) {
        Serial.printf("[Weather] API code: %d\n", code);
        return false;
    }

    JsonObject now = doc["now"];
    if (now.isNull()) return false;

    _data.temp = now["temp"] | 0.0f;
    _data.humidity = now["humidity"] | 0;
    _data.windSpeed = now["windSpeed"] | 0.0f;
    _data.windDir = now["windDirDegree"] | 0;
    _data.windScale = now["windScale"] | 0;

    // Icon and condition
    strncpy(_data.icon, now["icon"] | "999", sizeof(_data.icon) - 1);
    strncpy(_data.condition, getConditionText(_data.icon), sizeof(_data.condition) - 1);

    return true;
}

const char* WeatherClient::getWeatherIcon(const char* iconCode) {
    // Returns emoji-like icon strings for TFT display
    int code = atoi(iconCode);
    switch (code) {
        case 100: case 900: return "\x12";  // ☀ Sunny (using TFT symbols)
        case 101: case 102: case 103: return "\x13";  // 🌤 Cloudy
        case 104: return "\x14";  // ☁ Overcast
        case 305: case 306: case 307: return "\x15";  // 🌧 Light rain
        case 308: case 309: case 310: return "\x16";  // 🌧 Heavy rain
        case 400: case 401: case 402: return "\x17";  // ❄ Snow
        case 500: case 501: case 502: return "\x18";  // 🌫 Fog
        default: return "\x19";  // ❓ Unknown
    }
}

const char* WeatherClient::getConditionText(const char* iconCode) {
    int code = atoi(iconCode);
    // Return Chinese weather descriptions
    switch (code) {
        case 100: return "晴";
        case 101: return "多云";
        case 102: return "少云";
        case 103: return "晴间多云";
        case 104: return "阴";
        case 300: case 301: case 302: case 303: return "阵雨";
        case 304: return "雷阵雨";
        case 305: case 306: return "小雨";
        case 307: case 308: return "中雨";
        case 309: case 310: return "大雨";
        case 315: case 316: case 317: return "暴雨";
        case 400: return "小雪";
        case 401: return "中雪";
        case 402: return "大雪";
        case 500: case 501: case 502: return "雾";
        case 503: case 504: case 507: case 508: return "霾";
        default: return "未知";
    }
}
