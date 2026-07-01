/**
 * ESP32 Desktop Ornament
 *
 * An interactive desktop display that shows:
 *   - Clock (NTP-synced, digital + analog)
 *   - Weather (QWeather API)
 *   - Bilibili follower count & stats
 *   - Indoor temperature & humidity (DHT22)
 *
 * The screen auto-cycles through pages every 12 seconds.
 * Press the BOOT button (GPIO0) to manually switch pages.
 */

#include <Arduino.h>
#include "config.h"
#include "display/DisplayManager.h"
#include "network/WifiManager.h"
#include "network/BilibiliClient.h"
#include "network/WeatherClient.h"
#include "sensors/DHT22Sensor.h"
#include "time/TimeManager.h"
#include "ui/ScreenManager.h"

// ============================================================
// Global module instances
// ============================================================
DisplayManager   gDisplay;
WifiManager      gWifi;
TimeManager      gTime;
DHT22Sensor      gDHT22;
BilibiliClient   gBilibili;
WeatherClient    gWeather;
ScreenManager    gScreens;

// ============================================================
// Timing trackers (non-blocking)
// ============================================================
unsigned long gLastDHTRead  = 0;
unsigned long gLastNTP      = 0;
unsigned long gLastWeather  = 0;
unsigned long gLastBilibili = 0;

void setup() {
    Serial.begin(115200);
    delay(500);
    Serial.println("\n=========================================");
    Serial.println("   ESP32 Desktop Ornament v1.0");
    Serial.println("=========================================");

    // 1. Initialize display
    Serial.print("[Init] Display... ");
    if (gDisplay.begin(LANDSCAPE)) {
        Serial.println("OK");
    } else {
        Serial.println("FAILED");
    }

    // 2. Initialize WiFi (non-blocking)
    Serial.print("[Init] WiFi... ");
    gWifi.connectAsync();

    // 3. Initialize DHT22 sensor
    Serial.print("[Init] DHT22... ");
    if (gDHT22.begin()) {
        Serial.println("OK");
    } else {
        Serial.println("WARN (will retry in loop)");
    }

    // 4. Initialize NTP (needs WiFi, will sync later)
    gTime.begin();

    // 5. Initialize API clients
    gBilibili.setUID(BILIBILI_UID);
    gWeather.setLocation(QWEATHER_CITY_ID);

    // 6. Initialize screen manager
    gScreens.begin(gDisplay);

    // 7. Show boot notification
    gDisplay.showNotification("Starting up...", 1000, TFT_DARKGREEN);
    gDisplay.fadeIn(500);

    Serial.println("[Init] Setup complete!");
}

void loop() {
    unsigned long now = millis();

    // ============================================================
    // 1. Maintain WiFi connection
    // ============================================================
    bool wifiConnected = gWifi.maintain();

    // If WiFi just connected, sync time immediately
    if (wifiConnected && !gTime.isTimeValid() && (now - gLastNTP > 2000)) {
        gLastNTP = now;
        Serial.println("[Loop] WiFi ready, syncing NTP...");
        gTime.update();
    }

    // ============================================================
    // 2. Read DHT22 (every 5s)
    // ============================================================
    if (now - gLastDHTRead >= DHT22_UPDATE_INTERVAL_MS) {
        gLastDHTRead = now;
        gDHT22.read();
    }

    // ============================================================
    // 3. Update NTP time (every 1h)
    // ============================================================
    if (wifiConnected && gTime.isTimeValid() &&
        (now - gLastNTP >= NTP_UPDATE_INTERVAL_MS)) {
        gLastNTP = now;
        gTime.update();
    }

    // ============================================================
    // 4. Fetch weather (every 15 min)
    // ============================================================
    if (wifiConnected && (now - gLastWeather >= QWEATHER_UPDATE_INTERVAL_MS || gLastWeather == 0)) {
        gLastWeather = now;
        if (gWeather.fetch()) {
            Serial.println("[Loop] Weather updated");
        }
    }

    // ============================================================
    // 5. Fetch Bilibili stats (every 10 min)
    // ============================================================
    if (wifiConnected && (now - gLastBilibili >= BILIBILI_UPDATE_INTERVAL_MS || gLastBilibili == 0)) {
        gLastBilibili = now;
        if (gBilibili.fetch()) {
            Serial.println("[Loop] Bilibili data updated");
        }
    }

    // ============================================================
    // 6. Check BOOT button for manual page switch
    // ============================================================
    static bool lastBtnState = HIGH;
    static unsigned long lastBtnDebounce = 0;
    bool btnState = digitalRead(BTN_NEXT_PAGE);
    if (btnState == LOW && lastBtnState == HIGH && (now - lastBtnDebounce > BTN_DEBOUNCE_MS)) {
        lastBtnDebounce = now;
        gScreens.nextPage();
        Serial.printf("[Loop] Manual page switch → %d\n", gScreens.getCurrentPage());
    }
    lastBtnState = btnState;

    // ============================================================
    // 7. Render current screen
    // ============================================================
    gScreens.update(
        gDisplay,
        gTime,
        gDHT22.getData(),
        gWeather.getData(),
        gBilibili.getData(),
        wifiConnected
    );

    // ============================================================
    // 8. Brief delay to yield CPU
    // ============================================================
    delay(10);
}
