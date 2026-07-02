#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <TFT_eSPI.h>

// ============================================================
// WiFi Configuration
// ============================================================
#define WIFI_SSID       "your_wifi_ssid"
#define WIFI_PASSWORD   "your_wifi_password"
#define WIFI_TIMEOUT_MS 15000

// ============================================================
// NTP Configuration
// ============================================================
#define NTP_SERVER1     "ntp.aliyun.com"
#define NTP_SERVER2     "pool.ntp.org"
#define NTP_UPDATE_INTERVAL_MS  3600000  // 1 hour
#define TIMEZONE_OFFSET_SECS    28800    // UTC+8 (China)

// ============================================================
// API Configuration
// ============================================================

// Bilibili UID — find yours at https://space.bilibili.com/{UID}
#define BILIBILI_UID    "123456789"
#define BILIBILI_UPDATE_INTERVAL_MS  600000  // 10 minutes

// QWeather (和风天气) — register at https://dev.qweather.com
#define QWEATHER_API_KEY   "your_qweather_key"
#define QWEATHER_CITY_ID   "101010100"  // Default: Beijing
// Or use latitude/longitude: "116.41,39.92"
#define QWEATHER_UPDATE_INTERVAL_MS   900000  // 15 minutes

// ============================================================
// Sensor Configuration
// ============================================================
#define DHT22_PIN       15
#define DHT22_TYPE      DHT22
#define DHT22_UPDATE_INTERVAL_MS  5000  // 5 seconds

// ============================================================
// Display Configuration
// ============================================================
#define TFT_BL_PIN      4       // Backlight PWM pin
#define TFT_BL_CHANNEL  0       // LEDC channel
#define TFT_BRIGHTNESS  200     // 0-255
#define SCREEN_CYCLE_MS 12000   // 12 seconds per page
#define STATUS_BAR_H    20      // Status bar height in pixels

// ============================================================
// Button Configuration
// ============================================================
#define BTN_NEXT_PAGE   0       // BOOT button (GPIO0)
#define BTN_DEBOUNCE_MS 300

// ============================================================
// Shared Data Structures
// ============================================================

struct TimeData {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int weekday;       // 1=Monday ... 7=Sunday
    bool valid = false;

    // C-string buffers for display (filled by TimeManager)
    char dateStr[32];     // "2026年7月1日"
    char timeStr[16];     // "14:25:30"
    char weekdayStr[8];   // "星期三"
};

struct WeatherData {
    float temp;            // Current temperature (°C)
    int humidity;          // Relative humidity (%)
    float windSpeed;       // Wind speed (km/h)
    int windDir;           // Wind direction (degrees)
    int windScale;         // Wind scale (Beaufort)
    float tempMax;         // Daily max temperature
    float tempMin;         // Daily min temperature
    char condition[24];    // "晴", "阴", "小雨" etc.
    char icon[8];          // Weather icon code "100", "104" etc.
    bool valid = false;
    unsigned long lastUpdate = 0;
};

struct BilibiliData {
    long followerCount;     // 粉丝数
    long followingCount;    // 关注数
    long videoCount;        // 视频数
    long viewCount;         // 总播放量
    long likeCount;         // 总点赞数
    char userName[32];      // 昵称
    bool valid = false;
    unsigned long lastUpdate = 0;
};

struct IndoorData {
    float temperature;      // °C
    float humidity;         // %
    bool valid = false;
    unsigned long lastUpdate = 0;
};

#endif // CONFIG_H
