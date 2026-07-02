# 03-lcd-image — 图片背景桌面时钟

> **版本 v1.0** — 首个稳定版本，纯位图文字渲染，不依赖 drawString

在 2.4寸 ST7789 屏幕上显示：**背景图片 + 时间 + 星期 + 天气 + B站粉丝数**。

## 功能

- 🖼 **图片背景** — 240×320 RGB565 raw 图片，一次性刷入 SPIFFS
- ⏰ **NTP 时间** — 北京时间，60 秒刷新
- 🗓 **星期** — 英文缩写（Wed/Thu...）
- 🌤 **天气** — 心知天气 API，中文汉字（晴/多云/阴/雨/雷/雪/雾/霾/风）
- 📺 **B站粉丝数** — `api.bilibili.com`
- 🔄 **零闪烁** — 背景只推一次，后续只刷变化区域
- 🎯 **初始化** — 先显示 "Initializing..."，数据就绪后显示时钟

## 硬件接线

| 屏幕引脚# | 屏幕引脚 | → ESP32 |
|:---------:|:--------:|:-------:|
| 1 | GND | GND |
| 2 | RST | D15 |
| 3 | SCL | D2 |
| 4 | D/C | D4 |
| 5 | CS | D5 |
| 6 | SDA (MOSI) | D18 |
| 7 | SDO (MISO) | D19 |
| 8 | VCC | 3.3V |
| 9 | LEDA (背光+) | D21 |
| 10 | LEDK (背光-) | D22 |

## 配置

编辑 `src/main.cpp` 顶部配置区：

```cpp
const char *WIFI_SSID = "你的WiFi名";
const char *WIFI_PASS = "你的WiFi密码";
const char *BILIBILI_UID = "你的B站UID";
const char *WEATHER_KEY = "心知天气API密钥";
const char *WEATHER_CITY = "hangzhou";
```

### 获取心知天气 API Key

1. 注册 [心知天气](https://www.seniverse.com/)
2. 创建免费版应用，获取 API Key（`WEATHER_KEY`）
3. 默认城市 `hangzhou`，可按需修改

## 屏幕布局

```
┌──────────────────────────┐
│         Initializing...  │  ← 初始化时（白底）
└──────────────────────────┘

              ↓ 几秒后 ↓

┌──────────────────────────┐
│      ┌──────────────┐    │
│      │    11:11      │    │  ← 大号时间（居中）
│      │  Wed    晴     │    │  ← 星期 + 天气（同一行）
│      └──────────────┘    │
│       ┌──────────┐       │
│       │Fans: 14   │       │  ← 底部窄框
│       └──────────┘       │
└──────────────────────────┘
```

## 依赖库

- `TFT_eSPI` v2.5.43 — 屏幕驱动
- `ArduinoJson` v7.4.3 — API 解析
- `zimo.h` / `zimo.cpp` — 中文字模 20×20

## SPIFFS 说明

背景图片 `data/background.raw`（240×320 RGB565，153600 字节）需上传到 SPIFFS：

```bash
pio run --target uploadfs
```

如手动烧录：`esptool write-flash 0x290000 spiffs.bin`

## 编译 & 烧录

```bash
cd ~/quant-project/03-lcd-image
export PATH="$HOME/.local/bin:$PATH"

# 编译
pio run

# 烧录（三个文件）
cp .pio/build/featheresp32/*.bin /mnt/d/
# 然后在 Windows PowerShell 运行 esptool
```
