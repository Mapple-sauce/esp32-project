#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

enum WiFiState {
    WIFI_DISCONNECTED = 0,
    WIFI_CONNECTING,
    WIFI_CONNECTED,
    WIFI_ERROR
};

class WifiManager {
public:
    WifiManager();
    ~WifiManager();

    // Connect to WiFi (blocking with timeout)
    bool connect(const char* ssid = nullptr, const char* password = nullptr);

    // Non-blocking connection attempt (call in loop)
    bool connectAsync(const char* ssid = nullptr, const char* password = nullptr);

    // Check connectivity and reconnect if needed
    bool maintain();  // Returns true if connected

    // Disconnect
    void disconnect();

    // State
    WiFiState getState() const { return _state; }
    bool isConnected() const { return _state == WIFI_CONNECTED; }
    IPAddress getIP() const { return WiFi.localIP(); }
    const char* getSSID() const { return WiFi.SSID().c_str(); }
    int getRSSI() const { return WiFi.RSSI(); }

    // RSSI to signal strength level (0-3)
    int getSignalLevel() const;

private:
    WiFiState _state;
    unsigned long _lastReconnectAttempt;
    char _ssid[64];
    char _password[64];
    static void _onEvent(WiFiEvent_t event);
};

#endif // WIFI_MANAGER_H
