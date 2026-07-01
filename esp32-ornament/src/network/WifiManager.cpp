#include "WifiManager.h"

WifiManager::WifiManager()
    : _state(WIFI_DISCONNECTED)
    , _lastReconnectAttempt(0)
{
    _ssid[0] = '\0';
    _password[0] = '\0';
}

WifiManager::~WifiManager() {
    disconnect();
}

bool WifiManager::connect(const char* ssid, const char* password) {
    if (ssid) {
        strncpy(_ssid, ssid, sizeof(_ssid) - 1);
    } else {
        strncpy(_ssid, WIFI_SSID, sizeof(_ssid) - 1);
    }
    if (password) {
        strncpy(_password, password, sizeof(_password) - 1);
    } else {
        strncpy(_password, WIFI_PASSWORD, sizeof(_password) - 1);
    }

    _state = WIFI_CONNECTING;
    Serial.printf("[WiFi] Connecting to %s...\n", _ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > WIFI_TIMEOUT_MS) {
            Serial.println("[WiFi] Connection timeout!");
            _state = WIFI_ERROR;
            return false;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("[WiFi] Connected! IP: %s, RSSI: %d dBm\n",
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
    _state = WIFI_CONNECTED;
    return true;
}

bool WifiManager::connectAsync(const char* ssid, const char* password) {
    if (_state == WIFI_CONNECTED) return true;
    if (_state == WIFI_CONNECTING) return false;

    if (ssid) {
        strncpy(_ssid, ssid, sizeof(_ssid) - 1);
    } else {
        strncpy(_ssid, WIFI_SSID, sizeof(_ssid) - 1);
    }
    if (password) {
        strncpy(_password, password, sizeof(_password) - 1);
    } else {
        strncpy(_password, WIFI_PASSWORD, sizeof(_password) - 1);
    }

    _state = WIFI_CONNECTING;
    Serial.printf("[WiFi] Async connecting to %s...\n", _ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(_ssid, _password);
    return false;
}

bool WifiManager::maintain() {
    if (WiFi.status() == WL_CONNECTED) {
        if (_state != WIFI_CONNECTED) {
            _state = WIFI_CONNECTED;
            Serial.printf("[WiFi] Reconnected! IP: %s\n",
                          WiFi.localIP().toString().c_str());
        }
        return true;
    }

    // Not connected — try to reconnect
    if (_state == WIFI_CONNECTED || _state == WIFI_ERROR) {
        _state = WIFI_DISCONNECTED;
    }

    unsigned long now = millis();
    if (_state == WIFI_DISCONNECTED && (now - _lastReconnectAttempt > 10000)) {
        _lastReconnectAttempt = now;
        Serial.println("[WiFi] Attempting reconnection...");
        WiFi.reconnect();
        _state = WIFI_CONNECTING;
    }

    // Check if async connection succeeded
    if (_state == WIFI_CONNECTING && WiFi.status() == WL_CONNECTED) {
        _state = WIFI_CONNECTED;
        Serial.printf("[WiFi] Connected! IP: %s\n",
                      WiFi.localIP().toString().c_str());
        return true;
    }

    return false;
}

void WifiManager::disconnect() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    _state = WIFI_DISCONNECTED;
}

int WifiManager::getSignalLevel() const {
    int rssi = WiFi.RSSI();
    if (rssi > -50) return 3;
    if (rssi > -65) return 2;
    if (rssi > -80) return 1;
    return 0;
}
