/**
 * 03-lcd-image — 图片背景 + 信息叠加
 *
 * 功能:
 *   从 SPIFFS 加载 240x320 RGB565 图片作为背景
 *   叠加显示：星期、日期、时间、温度、B站粉丝数
 *   自动刷新时间，WiFi 断线重连
 *
 * 硬件: ESP32-WROOM (featheresp32)
 * 屏幕: 2.4" TFT ST7789, SPI 240x320
 *
 * 接线:
 *   引脚# | 屏幕引脚 | → ESP32
 *   ------|----------|--------
 *   1 GND   | GND      | GND
 *   2 RST   | RESET    | D15
 *   3 SCL   | SPI CLK  | D2
 *   4 D/C   | DATA/CMD | D4
 *   5 CS    | SPI CS   | D5
 *   6 SDA   | MOSI     | D18
 *   7 SDO   | MISO     | D19 (不接)
 *   8 VCC   | 3.3V     | 3.3V
 *   9 LEDA  | 背光 +   | D21
 *   10 LEDK | 背光 -   | D22
 */

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ============ 配置区 ============
const char *WIFI_SSID = "Xiaomi_wu";
const char *WIFI_PASS = "wu663366";
const char *BILIBILI_UID = "3461566076816331";
const int  REFRESH_INTERVAL = 30;  // 秒
const char *NTP_SERVER1 = "ntp.aliyun.com";
const char *NTP_SERVER2 = "cn.pool.ntp.org";
const char *NTP_SERVER3 = "time1.cloud.tencent.com";
constexpr long GMT_OFFSET_SEC = 8 * 3600;
constexpr int  DST_OFFSET_SEC = 0;

// ============ 引脚 ============
const int BACKLIGHT_NEG = 22;
TFT_eSPI tft;

// ============ 函数声明 ============
bool loadBackground();
void drawBackground();
void drawOverlay();
String getBeijingTime();
int getBilibiliFollower();
bool WiFi_Connect();
bool NTP_Setup();

// ============ setup ============
void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(BACKLIGHT_NEG, OUTPUT);
  digitalWrite(BACKLIGHT_NEG, LOW);

  tft.init();
  tft.setRotation(0);
  tft.writecommand(0x3A);
  tft.writedata(0x55);
  tft.invertDisplay(false);

  // 挂载 SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("[ERR] SPIFFS mount failed");
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.drawCentreString("SPIFFS Error", 120, 160, 2);
    return;
  }
  Serial.println("[OK] SPIFFS mounted");

  // 加载背景图片
  if (!loadBackground()) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("No background image", 120, 160, 2);
    tft.drawCentreString("upload data/ to SPIFFS", 120, 190, 2);
  }

  // WiFi + NTP（后台静默连接）
  WiFi_Connect();
  if (WiFi.status() == WL_CONNECTED) NTP_Setup();
}

// ============ loop ============
void loop() {
  static unsigned long lastRefresh = 0;
  unsigned long now = millis();

  if (now - lastRefresh >= REFRESH_INTERVAL * 1000UL) {
    lastRefresh = now;

    // WiFi 保活
    if (WiFi.status() != WL_CONNECTED) WiFi_Connect();

    // 重绘背景 + 叠加信息
    drawBackground();
    drawOverlay();
  }
}

// ============ 背景图片加载 ============

bool loadBackground() {
  // 后续实现：从 SPIFFS 读取 background.raw
  // 格式：240*320*2 = 153600 字节 RGB565
  return false;
}

void drawBackground() {
  // 后续实现：将 background.raw 直接刷到屏幕
}

// ============ 信息叠加 ============

void drawOverlay() {
  // 在背景上叠加文字
}

// ============ WiFi ============

bool WiFi_Connect() {
  Serial.print("[WIFI] Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (++attempts > 40) {
      Serial.println(" TIMEOUT");
      return false;
    }
  }
  Serial.println(" OK");
  Serial.printf("  IP: %s\n", WiFi.localIP().toString().c_str());
  return true;
}

// ============ NTP ============

bool NTP_Setup() {
  Serial.print("[NTP] Syncing");
  configTime(GMT_OFFSET_SEC, DST_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
  for (int i = 0; i < 50; ++i) {
    if (time(nullptr) > 1700000000) { Serial.println(" OK"); return true; }
    delay(200);
    Serial.print(".");
  }
  Serial.println(" TIMEOUT");
  return false;
}

String getBeijingTime() {
  struct tm t;
  if (!getLocalTime(&t)) return "";
  char buf[64];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %a", &t);
  return String(buf);
}

// ============ B站 API ============

int getBilibiliFollower() {
  HTTPClient http;
  String url = String("https://api.bilibili.com/x/relation/stat?vmid=") + BILIBILI_UID;
  http.begin(url);
  http.setTimeout(5000);
  http.setUserAgent("Mozilla/5.0");
  int code = http.GET();
  if (code != 200) { http.end(); return -1; }
  String response = http.getString();
  http.end();
  JsonDocument doc;
  if (deserializeJson(doc, response)) return -2;
  if (doc["code"].as<int>() != 0) return -3;
  return doc["data"]["follower"].as<int>();
}
