# 01-wifi — WiFi + NTP + B站粉丝数

连接 WiFi 获取北京时间，并调用 B站 API 获取粉丝数，串口每 30 秒刷新显示。

## 功能

- 📶 WiFi 连接（WPA2 个人）
- 🕐 NTP 获取北京时间（阿里云 NTP）
- 📺 B站粉丝数（`api.bilibili.com/x/relation/stat`）
- 🔄 每 30 秒自动刷新
- 📡 WiFi 断开自动重连

## 串口输出示例

```
========================================
  01-wifi - WiFi + NTP + Bilibili
========================================
  [WIFI] Connecting to Xiaomi_wu
...... OK
  [WIFI] IP: 192.168.1.100
  [NTP] Syncing time.......... OK

[DATA] Initial fetch:
  Beijing Time: 2026-07-01 17:06:47 Wed
  Bilibili Followers: 14

========================================
Auto-refresh every 30s

  [NTP] 2026-07-01 17:06:47 Wed
  [BILI] Followers: 14
  ---
```

## 配置

如需修改 WiFi 或 B站 UID，编辑 `src/main.cpp` 顶部配置区：

```cpp
// WiFi
const char *WIFI_SSID = "你的WiFi名";
const char *WIFI_PASS = "你的WiFi密码";

// B站 UID
const char *BILIBILI_UID = "你的B站UID";

// 刷新间隔（秒）
const int REFRESH_INTERVAL = 30;
```

## 依赖库

- `WiFi.h`（ESP32 内置）
- `HTTPClient.h`（ESP32 内置）
- `ArduinoJson` v7.4.3（PlatformIO 管理）
- `time.h`（ESP32 内置）

## Flash 占用

- RAM: 14.4% (47KB)
- Flash: 71.0% (930KB)
