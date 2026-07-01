/**
 * 01-wifi
 * 功能：连接WiFi → NTP获取北京时间 → B站API获取粉丝数
 * 串口打印所有信息，每30秒自动刷新
 *
 * 硬件: ESP32-WROOM (featheresp32)
 */

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

// ============================================================
// 配置区 — 按需修改
// ============================================================

// WiFi
const char *WIFI_SSID = "Xiaomi_wu";
const char *WIFI_PASS = "wu663366";

// NTP
constexpr long GMT_OFFSET_SEC = 8 * 3600;   // UTC+8 北京时间
constexpr int  DST_OFFSET_SEC = 0;
const char *NTP_SERVER1 = "ntp.aliyun.com";
const char *NTP_SERVER2 = "cn.pool.ntp.org";
const char *NTP_SERVER3 = "time1.cloud.tencent.com";

// B站
const char *BILIBILI_UID = "3461566076816331";

// 刷新间隔（秒）
const int REFRESH_INTERVAL = 30;

// LED
const int LED_PIN = 2;

// ============================================================
// 函数声明
// ============================================================

bool   WiFi_Connect();
bool   NTP_Setup();
String getBeijingTime();
int    getBilibiliFollower();
void   printSeparator();

// ============================================================
// setup
// ============================================================

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LED_PIN, OUTPUT);

  Serial.println("\n");
  printSeparator();
  Serial.println("  01-wifi - WiFi + NTP + Bilibili");
  printSeparator();

  // 1) 连接 WiFi
  if (WiFi_Connect()) {
    // 2) NTP 时间同步
    NTP_Setup();
  } else {
    Serial.println("[FAIL] WiFi connection failed, continuing without network");
  }

  // 3) 首次数据获取
  Serial.println("");
  Serial.println("[DATA] Initial fetch:");
  Serial.println("");

  // 显示北京时间
  String now = getBeijingTime();
  if (now.length() > 0) {
    Serial.print("  Beijing Time: ");
    Serial.println(now);
  }

  // 显示B站粉丝数
  int followers = getBilibiliFollower();
  if (followers >= 0) {
    Serial.print("  Bilibili Followers: ");
    Serial.print(followers);
  } else {
    Serial.print("  Bilibili: Failed (code=");
    Serial.print(followers);
    Serial.print(")");
  }
  Serial.println("");

  Serial.println("");
  printSeparator();
  Serial.print("Auto-refresh every ");
  Serial.print(REFRESH_INTERVAL);
  Serial.println("s\n");

  // LED 快闪 3 次表示启动完成
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH); delay(100);
    digitalWrite(LED_PIN, LOW);  delay(100);
  }
}

// ============================================================
// loop
// ============================================================

void loop() {
  static unsigned long lastRefresh = 0;
  unsigned long now = millis();

  // 每 REFRESH_INTERVAL 秒刷新一次
  if (now - lastRefresh >= REFRESH_INTERVAL * 1000UL) {
    lastRefresh = now;

    // LED 闪烁指示刷新中
    digitalWrite(LED_PIN, HIGH);

    // 检查 WiFi 状态，断开则重连
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("[WARN] WiFi disconnected, reconnecting...");
      WiFi_Connect();
    }

    // 获取时间
    String timeStr = getBeijingTime();
    if (timeStr.length() > 0) {
      Serial.print("  [NTP] ");
      Serial.println(timeStr);
    } else {
      Serial.println("  [NTP] Not synced");
    }

    // 获取B站粉丝
    int followers = getBilibiliFollower();
    if (followers >= 0) {
      Serial.print("  [BILI] Followers: ");
      Serial.println(followers);
    } else if (followers == -1) {
      Serial.println("  [BILI] HTTP failed");
    } else if (followers == -2) {
      Serial.println("  [BILI] JSON parse failed");
    } else if (followers == -3) {
      Serial.println("  [BILI] API error");
    }

    Serial.println("  ---");
    Serial.println("");

    digitalWrite(LED_PIN, LOW);
  }
}

// ============================================================
// WiFi 连接
// ============================================================

bool WiFi_Connect() {
  Serial.print("  [WIFI] Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    attempts++;
    if (attempts > 40) {  // 20秒超时
      Serial.println("");
      Serial.println("  [WIFI] Timeout");
      return false;
    }
  }

  Serial.println(" OK");
  Serial.print("  [WIFI] IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

// ============================================================
// NTP 时间同步
// ============================================================

bool NTP_Setup() {
  Serial.print("  [NTP] Syncing time");

  configTime(GMT_OFFSET_SEC, DST_OFFSET_SEC,
             NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);

  // 等待 SNTP 同步（最多等 10 秒）
  for (int i = 0; i < 50; ++i) {
    time_t now = time(nullptr);
    if (now > 1700000000) {  // > 2023-11-14，说明已同步
      Serial.println(" OK");
      return true;
    }
    delay(200);
    Serial.print(".");
  }

  Serial.println(" TIMEOUT");
  return false;
}

// ============================================================
// 获取北京时间
// ============================================================

String getBeijingTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    return String();
  }
  char buf[64];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S %a", &timeinfo);
  return String(buf);
}

// ============================================================
// 获取 B站 粉丝数
// ============================================================
// 返回值: >=0 粉丝数, -1 HTTP失败, -2 JSON解析失败, -3 API错误

int getBilibiliFollower() {
  HTTPClient http;

  String url = String("https://api.bilibili.com/x/relation/stat?vmid=") + BILIBILI_UID;
  http.begin(url);
  http.setTimeout(5000);

  // 设置 User-Agent 避免被拒
  http.setUserAgent("Mozilla/5.0");

  int httpCode = http.GET();

  if (httpCode != 200) {
    Serial.printf("  [BILI] HTTP error: %d\n", httpCode);
    http.end();
    return -1;
  }

  String response = http.getString();
  http.end();

  // 解析 JSON
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.printf("  [BILI] JSON parse error: %s\n", error.c_str());
    return -2;
  }

  int code = doc["code"].as<int>();
  if (code != 0) {
    Serial.printf("  [BILI] API error code: %d, msg: %s\n",
                  code, doc["message"].as<const char *>());
    return -3;
  }

  return doc["data"]["follower"].as<int>();
}

// ============================================================
// 工具函数
// ============================================================

void printSeparator() {
  Serial.println("========================================");
}
